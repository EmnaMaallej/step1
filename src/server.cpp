#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>

#define SERVICE_ID 0x1234
#define INSTANCE_ID 0x5678
#define METHOD_ID 0x0421
#define EVENT_ID 0x8001

class Server {
public:
    Server() {
        app_ = vsomeip::runtime::get()->create_application("server-app");
    }

    void init() {
        app_->init(); // Appel sans vérification de retour (géré par vsomeip)
        app_->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID,
            std::bind(&Server::on_request, this, std::placeholders::_1));
        app_->register_state_handler(
            std::bind(&Server::on_state, this, std::placeholders::_1));
        app_->offer_service(SERVICE_ID, INSTANCE_ID);
        app_->offer_event(SERVICE_ID, INSTANCE_ID, EVENT_ID, {EVENT_ID}, false);
    }

    void start() {
        std::thread notification_thread(&Server::send_notification, this);
        app_->start();
        notification_thread.join();
    }

private:
    void on_state(vsomeip::state_type_e _state) {
        if (_state == vsomeip::state_type_e::ST_REGISTERED) {
            app_->offer_service(SERVICE_ID, INSTANCE_ID); // Offre le service ici
        }
    }

    void on_request(const std::shared_ptr<vsomeip::message> &_request) {
        std::cout << "Received request: " << _request->get_payload()->get_data() << std::endl;
        auto response = vsomeip::runtime::get()->create_response(_request);
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string response_data = "Response from server";
        payload->set_data((const vsomeip::byte_t*)response_data.data(), response_data.size());
        response->set_payload(payload);
        app_->send(response);
    }

    void send_notification() {
        while (true) {
            auto payload = vsomeip::runtime::get()->create_payload();
            std::string notification_data = "Notification from server";
            payload->set_data((const vsomeip::byte_t*)notification_data.data(), notification_data.size());
            app_->notify(SERVICE_ID, INSTANCE_ID, EVENT_ID, payload);
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    std::shared_ptr<vsomeip::application> app_;
};

int main() {
    Server server;
    server.init();
    server.start();
    return 0;
}
