#include <vsomeip/vsomeip.hpp>
#include <iostream>

#define SERVICE_ID 0x1234
#define INSTANCE_ID 0x5678
#define METHOD_ID 0x0421
#define EVENT_ID 0x8001

class Client {
public:
    Client() {
        app_ = vsomeip::runtime::get()->create_application("client-app");
    }

    void init() {
        app_->init();
        app_->register_message_handler(SERVICE_ID, INSTANCE_ID, METHOD_ID,
            std::bind(&Client::on_response, this, std::placeholders::_1));
        app_->register_message_handler(SERVICE_ID, INSTANCE_ID, EVENT_ID,
            std::bind(&Client::on_notification, this, std::placeholders::_1));
        app_->register_state_handler(
            std::bind(&Client::on_state, this, std::placeholders::_1));
        app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
            std::bind(&Client::on_availability, this, std::placeholders::_1, std::placeholders::_2));
        app_->subscribe(SERVICE_ID, INSTANCE_ID, EVENT_ID);
    }

    void start() {
        app_->start();
    }

    void send_request() {
        auto message = vsomeip::runtime::get()->create_request();
        message->set_service(SERVICE_ID);
        message->set_instance(INSTANCE_ID);
        message->set_method(METHOD_ID);
        auto payload = vsomeip::runtime::get()->create_payload();
        std::string request_data = "Request from client";
        payload->set_data((const vsomeip::byte_t*)request_data.data(), request_data.size());
        message->set_payload(payload);
        app_->send(message);
    }

private:
    void on_state(vsomeip::state_type_e _state) {
        if (_state == vsomeip::state_type_e::ST_REGISTERED) {
            app_->request_service(SERVICE_ID, INSTANCE_ID);
        }
    }

    void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
        if (_is_available) {
            std::cout << "Service available, sending request..." << std::endl;
            send_request();
        }
    }

    void on_response(const std::shared_ptr<vsomeip::message> &_response) {
        std::cout << "Received response: " << _response->get_payload()->get_data() << std::endl;
    }

    void on_notification(const std::shared_ptr<vsomeip::message> &_notification) {
        std::cout << "Received notification: " << _notification->get_payload()->get_data() << std::endl;
    }

    std::shared_ptr<vsomeip::application> app_;
};

int main() {
    Client client;
    client.init();
    client.start();
    return 0;
}
