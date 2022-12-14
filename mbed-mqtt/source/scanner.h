#include <cstdlib>
#include <events/mbed_events.h>

#include <cstdio>

#include "BLETypes.h"
#include "ble/BLE.h"
#include "gap/AdvertisingDataParser.h"
#include "gap/Gap.h"
#include "mbed.h"
#include "pretty_printer.h"

extern events::EventQueue event_queue;
extern PwmOut led;

static const ble::ScanParameters scan_params(
            ble::phy_t::LE_1M,
            ble::scan_interval_t(1600),
            ble::scan_window_t(40),
            false /* active scanning */
        );

static const ble::scan_duration_t scan_duration(ble::millisecond_t(900));

class Scanner : private mbed::NonCopyable<Scanner>, public ble::Gap::EventHandler {
   public:
    Scanner(BLE& ble, events::EventQueue& event_queue)
        : _ble(ble), _gap(ble.gap()), _event_queue(event_queue), _led1(LED1, 0) {}

    ~Scanner() {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
        }
    }

    void run() {
        if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
        }

        /* handle gap events */
        _gap.setEventHandler(this);

        ble_error_t error = _ble.init(this, &Scanner::on_init_complete);
        if (error) {
            print_error(error, "Error returned by BLE::init\r\n");
            return;
        }

        /* to show we're running we'll blink every 500ms */
        _event_queue.call_every(500ms, this, &Scanner::blink);

        /* this will not return until shutdown */
        // _event_queue.dispatch_forever();
    }

    void start_scan() {
        _event_queue.call(this, &Scanner::scan);
    }

    void stop_scan() {
        _gap.stopScan();
    }

   public:
    bool phone_near = false;

   private:
    void on_init_complete(BLE::InitializationCompleteCallbackContext* event) {
        if (event->error) {
            print_error(event->error, "Error during the initialisation\r\n");
            return;
        }

        print_mac_address();

        /* all calls are serialised on the user thread through the event queue */
        // _event_queue.call_every(1000ms, this, &Scanner::scan);
        _event_queue.call(this, &Scanner::scan);
    }

    void scan() {
        // phone_near = false;
        // ble::ScanParameters scan_params;
        // scan_params.setOwnAddressType(ble::own_address_type_t::RANDOM);

        ble_error_t error = _gap.setScanParameters(scan_params);

        if (error) {
            print_error(error, "Error caused by Gap::setScanParameters\r\n");
            return;
        }

        
        printf("start scan\n");
        error = _gap.startScan();

        if (error) {
            print_error(error, "Error caused by Gap::startScan\r\n");
            return;
        }
    }

    virtual void onAdvertisingReport(const ble::AdvertisingReportEvent& event) {
        // printf("onAdvertisingReport\n");
        static int count = 0;
        static int mac_cache;

        if (event.getRssi() < -60) return;

        // count++;
        // if (count % 3 != 0) {
        //     return;
        // }

        // printf("o\n");

        ble::address_t address = event.getPeerAddress();
        // printf("%d\n", address[5]);
        if (mac_cache != 0 && address[5] == mac_cache) {
        // print_address(address);
            // printf("cache\n");
            phone_near = true;
            return;
        }

        ble::AdvertisingDataParser adv_parser(event.getPayload());

        /* parse the advertising payload, looking for a discoverable device */
        while (adv_parser.hasNext()) {
            ble::AdvertisingDataParser::element_t field = adv_parser.next();

            // printf("Type: %d, Value: %s\n", field.type, field.value.data());

            // printf("???: %s\n", field.value.data());
            if (field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME) {
                // printf("Name: %s\n", field.value.data());
                // print_address(event.getPeerAddress());

                if (field.value.data()[0] == 'T') {
                    // printf("RSSI: %d\n", event.getRssi());
                    mac_cache = address[5];
                    phone_near = true;
                    return;
                }
            }
        }
    }

    void blink(void) { _led1 = !_led1; }

   private:
    BLE& _ble;
    ble::Gap& _gap;
    events::EventQueue& _event_queue;

    DigitalOut _led1;
};

// int main() {
//     BLE& ble = BLE::Instance();

//     ble.onEventsToProcess(schedule_ble_events);

//     Scanner scanner(ble, event_queue);

//     scanner.run();

//     while (true) {
//     }
// }
