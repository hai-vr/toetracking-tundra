
#include "tundra_mapped_input.h"

#define RP2040_BB_LED           (25)
#define ADC_BIGTOE              (27) // (26) // GPIO27_ADC1 (schematic is wrong, this is pin 5 on the IO0 socket,
                                             // see https://twitter.com/ImmersiveDevice/status/1755296026301235333)
#define ADC_SECONDPHALANX       (28) // (27) // GPIO28_ADC2 (schematic is wrong, this is pin 4 on the IO0 socket)
#define ADC_SPLAY               (29) // (28) // GPIO29_ADC3 (schematic is wrong, this is pin 2 on the IO0 socket)

// Create TMI object to communicate with Tundra Tracker
TMI tundra_tracker;
void csn_irq( uint gpio, uint32_t event_mask );  // Chip select IRQ must be top level so let's make one and connect it later in setup

// Use packed structs to define the inputs for your controller
// First we make a struct of buttons
typedef struct __attribute__( ( packed, aligned( 1 ) ) )
{

}
controller_buttons_t;

// Make a top level struct that packs the button data along with the rest of the controller analog values
//typedef struct __attribute__( ( packed, aligned( 1 ) ) )
//{
//  uint8_t       system          : 1;  //0
//  uint8_t       a               : 1;  //1
//  uint8_t       b               : 1;  //2
//  uint8_t       trigger_btn     : 1;  //3
//  uint8_t       grip            : 1;  //4
//  uint8_t       thumbstick      : 1;  //5
//  uint8_t       menu            : 1;  //6
//  uint8_t       thumbstick_en   : 1;  //7
//  uint16_t      thumbstick_x    : 10; //8
//  uint16_t      thumbstick_y    : 10; //18
//  uint16_t      trigger         : 10; //28
//  uint16_t      index           : 10; //38
//  uint16_t      middle          : 10; //48
//  uint16_t      ring            : 10; //58
//  uint16_t      pinky           : 10; //68
//}
typedef struct __attribute__( ( packed, aligned( 1 ) ) )
{
    uint8_t       system          : 1;  //0
    uint8_t       bigtoe_is_over_secondtoe : 1;  //1
    uint16_t      bigtoe_curl           : 10; //2
    uint16_t      secondtoe_curl           : 10; //12
    uint16_t      fifthtoe_splay           : 10; //22
//    uint8_t       b               : 1;  //2
//    uint8_t       trigger_btn     : 1;  //3
//    uint8_t       grip            : 1;  //4
//    uint8_t       thumbstick      : 1;  //5
//    uint8_t       menu            : 1;  //6
//    uint8_t       thumbstick_en   : 1;  //7
//    uint16_t      thumbstick_x    : 10; //8
//    uint16_t      thumbstick_y    : 10; //18
//    uint16_t      trigger         : 10; //28
//    uint16_t      index           : 10; //38
//    uint16_t      middle          : 10; //48
//    uint16_t      ring            : 10; //58
//    uint16_t      pinky           : 10; //68
}
controller_data_t;

// declare the controller struct for use
controller_data_t controller_data;

// Some other variables to blink an LED
uint32_t interval = 100; //refresh time in ms
uint8_t increment = 10;
uint32_t next_time = 0;
bool led_state = false;

void setup() {
  
  pinMode(RP2040_BB_LED, OUTPUT);

  // init the communication to Tundra Tracker, setup the CS irq callback (this has to be at Top level in arduino it seems)
  tundra_tracker.init( );
  gpio_set_irq_enabled_with_callback( tundra_tracker.get_cs_pin(), GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &csn_irq );

//  controller_data.thumbstick_en = 1;
  //for demo only,

//    adcAttachPin(ADC_SPLAY);
//    analogSetAttenuation(ADC_11db);
//    analogSetWidth(12);
//    analogReadResolution(12);
  //  pinMode(ADC_BIGTOE, INPUT_PULLUP);
  //  pinMode(ADC_SECONDPHALANX, INPUT_PULLUP);
//    pinMode(ADC_SPLAY, INPUT_PULLUP);
    // analogReadResolution(12);
}

float ioAval;
float ioBval;
float ioCval;
float ioAcomp_prev;
float ioCval_prev;

float Comp(float input, float mul, float min, float max) {
    float result = (input - min * mul) / (max * mul - min * mul);
    if (result < 0) result = 0;
    if (result > 1) result = 1;
    result = 1 - result;
    return result;
}

bool toeIsOverSecond = false;

int k = 0;
// the loop function runs over and over again forever
void loop() {
  // Flag will be true when the library is ready for new data
  if ( tundra_tracker.data_ready() )
  {
    // Copy our controller struct to the data packet
    tundra_tracker.send_data( &controller_data, sizeof(controller_data) );

    // House keeping function for the library that should be ran right after data is ready
    tundra_tracker.handle_rx_data( );
    k++;
    if (k % 100 == 0) {
        digitalWrite(RP2040_BB_LED, led_state ^= true);
    }
  }

  // Framework for a subroutine that runs every 250ms, not nessesary for all examples but slows down how fast our data is incrementing and allows us
  // to blink an LED at human speeds
  if ( millis() > next_time )
  {


    // Simulate some changing controller data just by incrementing and toggling
    // This is the start of demo code, delete this if you do not want simulated input data
    // ----------------Begin input data simulation ----------------
//    controller_data.a = led_state;
//    controller_data.trigger +=increment;
//
//    if ( controller_data.index <= 1000 )
//      controller_data.index+=increment;
//    else if ( controller_data.middle <= 1000 )
//      controller_data.middle+=increment;
//    else if ( controller_data.ring <= 1000 )
//      controller_data.ring+=increment;
//    else if ( controller_data.pinky <= 1000 )
//      controller_data.pinky+=increment;
//    else
//    {
//      controller_data.index = 0;
//      controller_data.middle = 0;
//      controller_data.ring = 0;
//      controller_data.pinky = 0;
//    }
//
//    if ( controller_data.thumbstick_x <= 1000 )
//      controller_data.thumbstick_x += increment;
//    else
//    {
//      controller_data.thumbstick_x = 0;
//      controller_data.thumbstick_y += increment;
//    }

    // ----------------End of input data simulation ----------------

    // Twiddle the LED
//    digitalWrite(RP2040_BB_LED, led_state ^= true);

    // Setup the next refresh time
    next_time = millis() + interval;
  }

    
      // float xxxxyyyy = analogRead(ADC_SPLAY);
      ioAval = analogRead(ADC_BIGTOE);
      ioBval = analogRead(ADC_SECONDPHALANX);
      ioCval = analogRead(ADC_SPLAY);
      if (ioAval < 0) ioAval = 0;
      if (ioBval < 0) ioBval = 0;
      if (ioCval < 0) ioCval = 0;
      ioAval = (float)(ioAval / 290);
      ioBval = (float)(ioBval / 315);
      ioCval = (float)(ioCval / 310);
  //  float ioAcomp = Comp(ioAval, 0.45f, 0.6f);
  //  float ioBcomp = Comp(ioBval, 0.35f, 0.6f);
  //  float ioCcomp = Comp(ioCval, 0.52f, 0.6f);
  float fx = 1;
  //  float ioAcomp = Comp(ioAval, fx, 0.45f, 0.6f);
  //  float ioBcomp = Comp(ioBval, fx, 0.35f, 0.6f);
  //  float ioCcomp = Comp(ioCval, fx, 0.52f, 0.6f);
  //  float ioAcomp = Comp(ioAval, fx, 0.75f, 1.0f); // 0.75 = 0.45 * (1 / 0.6f)
  //  float ioBcomp = Comp(ioBval, fx, 0.584f, 1.0f);
  //  float ioCcomp = Comp(ioCval, fx, 0.867f, 1.0f);
   float ioAcomp = Comp(ioAval, fx, 0.7, 1.0f); // 0.75 = 0.45 * (1 / 0.6f)
   float ioBcomp = Comp(ioBval, fx, 0.5, 1.0f);
//   float ioCcomp = Comp(ioCval, fx, 0.5, 1.0f);
    float ioCcomp = Comp(ioCval, fx, 0.75, 0.9f);
      controller_data.bigtoe_curl = ioAcomp * 1023;
      controller_data.secondtoe_curl = ioBcomp * 1023;
      controller_data.fifthtoe_splay = ioCcomp * 1023;


if (!toeIsOverSecond) {
    if (ioAcomp_prev <= 0.15 && ioAcomp > 0.15 && ioBcomp > 0.55) {
        toeIsOverSecond = true;
    }
}
else {
    if (ioAcomp < 0.13) toeIsOverSecond = false;
    else if (ioBcomp < 0.3 && ioAcomp < 0.45) toeIsOverSecond = false;
}
    controller_data.bigtoe_is_over_secondtoe = toeIsOverSecond ? 1 : 0;
    controller_data.system = 0;
    ioAcomp_prev = ioAcomp;

//      if (ioCval != ioCval_prev) {
//        ioCval_prev = ioCval;
//          controller_data.grip = !controller_data.grip;
//      }
}

// Callback for SPI Chipselect, just connect in the tmi irq function
void csn_irq( uint gpio, uint32_t event_mask )
{
  tundra_tracker.csn_irq( gpio, event_mask );
}
