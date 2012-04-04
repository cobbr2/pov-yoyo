
// FAIL: looks like it just PWMs the LED at dim level.
// but that's the time we have today.
// set up for serial monitoring and swing the thing around a bit!

// hardware setup
const int BADASS_ACCEL = 0; // on analog port 0

// going for ability to have first image be grey-scale by using
// analog outputs (also, that's how RGBike works). Current setup
// is just one digital LED to get it going though.... using 13 so
// I don't even have to bend it up :)
const int LED_COUNT = 1;
const byte LED[LED_COUNT] = { 13 };


// ring buffer of samples taken so far; hope is to read these out
// after a run if I can still talk serial after going to battery &
// back.
const int SAMPLE_BUFFER_SIZE = 4096;
int last_samples[SAMPLE_BUFFER_SIZE];
unsigned int sample_ix = 0;

// image setup
// first version is the simplest image and worst resolution possible: one LED,
// which I want on for half a revolution.
const byte MAXCOLORS = 1;
const byte ROWS_PER_IMAGE = 30;

unsigned int row_ix = 0;
int image[ROWS_PER_IMAGE][MAXCOLORS][LED_COUNT] = {
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { HIGH } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
   { { LOW  } },
};

// rate control parameters
unsigned int wait_ix = 0;
unsigned int waits_per_line = 1000;      // just a guess to start with, should train.
unsigned int frames_displayed = 0;

void setup() {
    // using digital for the moment, just to get running
   for (int ix = 0; ix < LED_COUNT; ++ix) {
       pinMode(LED[ix],OUTPUT);
   }
   Serial.begin(9600);
}

// this algo might work for spinning on the end of a string,
// but the slow-down flag isn't good for dealing with freefall
// where we'll have a steady set of zeros. I.e., we could get our
// next sample in 3.75 frames, which would be too *slow*, but
// slow down more because the frame count is non-zero.  Need
// to have a better prediction idea... maybe fractional frames
// would actually work.
void frame_start() {
  if (frames_displayed > 0) {
      // assume we need to slow down, see comments about freefall the divide by
      // six seems to be a smoothing operator, stealing directly, which
      // requires rows / frame to be >> 6
      waits_per_line += row_ix / 6;
      frames_displayed = 0;
  } else {
      // speed up.
      waits_per_line -= (ROWS_PER_IMAGE - row_ix) / 6;
      if (waits_per_line <= 0) waits_per_line = 1;
  }
  row_ix = 0;
}

int current_trend = 0;
int max = 506 ;
int min = 502 ;
int fuzz = 1;
int val = 0;
// separated sensing the event from responding to it. This
// is sensing, obviously
void check_accelerometer() {

  val = analogRead(BADASS_ACCEL);
//  int lastval = last_samples[sample_ix];

  Serial.println(val);
  delay(100);    // slow the whole thing down for a bit while we're flicking

//  if (val >= (max - fuzz)) {
//      Serial.print(val);
//      Serial.println(" New ");
//      if (val > max) {
//        max = val;
//      }
//      max = val;
//      frame_start;
//  } else if (val <= (min + fuzz)) {
//      Serial.print(val);
//      Serial.println(" weN ");
//      // current_trend = -1;
//  } else if (val == lastval) {
//  //    current_trend = 0;
//  }
//
//  last_samples[sample_ix++] = val;
//  if (sample_ix > SAMPLE_BUFFER_SIZE) {
//    sample_ix = 0;
//  }
}

// color is intended to address specific set of LEDs eventually,
// ignored now.
void output_to_leds(byte color,int values[]) {
   for (int ix = 0; ix < LED_COUNT; ++ix) {
     digitalWrite(LED[ix],values[ix]);
     // analogWrite(LED[ix],values[ix])
   }
}

void output_current_line() {
  for (int color_ix = 0; color_ix < MAXCOLORS; ++color_ix) {

      // if this was using the ti chip we'd do
      // light_up(COLOR[color_ix-1 % 3]) here (SBI(COLOUR_PORT... stuff)
      // then we'd blank the blank_port, then

      output_to_leds(color_ix, image[row_ix][color_ix]);
  }
}

void count() {
  wait_ix++;
  if (wait_ix >= waits_per_line) {
      wait_ix = 0;
      row_ix++;
      if (row_ix >= ROWS_PER_IMAGE) {
          row_ix = 0;
          frames_displayed++;
      }
  }
}

void loop() {

  /* design assumes sensing takes no time... we'll see */

  check_accelerometer;

  // output_current_line;

  // count;
}
