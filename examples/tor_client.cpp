/*********************************************************************
  This is an Tor client example for Monochrome OLEDs based on SSD1306 drivers

  This example is for a 128x32|64 size display using SPI or I2C to communicate
  4 or 5 pins are required to interface

  Written by @Hpsaturn, fork from Limor Fried/Ladyada  for Adafruit Industries.  
  BSD license, check license.txt for more information

  2015.10.07  @Hpsaturn   Initial test commands with tor interface.
  2013.02.18 	Charles-Henri Hallard (http://hallard.me)
  Modified for compiling and use on Raspberry ArduiPi Board
  LCD size and connection are now passed as arguments on 
  the command line (no more #define on compilation needed)
  ArduiPi project documentation http://hallard.me/arduipi


 *********************************************************************/

#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"

#include <getopt.h>

#define PRG_NAME        "raspi tor client demo"
#define PRG_VERSION     "0.1"

// Instantiate the display
ArduiPi_OLED display;

// Config Option
struct s_opts
{
    int oled;
    int verbose;
} ;

int sleep_divisor = 1 ;

// default options values
s_opts opts = {
    OLED_ADAFRUIT_SPI_128x32,	// Default oled
    false						// Not verbose
};

void scrollText(){

    display.startscrollright(0x00, 0x0F);
    sleep(2);
    display.stopscroll();
    sleep(1);
    display.startscrollleft(0x00, 0x0F);
    sleep(2);
    display.stopscroll();
    sleep(1);    
    display.startscrolldiagright(0x00, 0x07);
    sleep(2);
    display.startscrolldiagleft(0x00, 0x07);
    sleep(2);
    display.stopscroll();

}

/* ======================================================================
Function: usage
Purpose : display usage
Input 	: program name
Output	: -
Comments: 
====================================================================== */
void usage( char * name)
{
    printf("%s\n", name );
    printf("Usage is: %s --oled type [options]\n", name);
    printf("  --<o>led type\nOLED type are:\n");
    for (int i=0; i<OLED_LAST_OLED;i++)
        printf("  %1d %s\n", i, oled_type_str[i]);

    printf("Options are:\n");
    printf("  --<v>erbose  : speak more to user\n");
    printf("  --<h>elp\n");
    printf("<?> indicates the equivalent short option.\n");
    printf("Short options are prefixed by \"-\" instead of by \"--\".\n");
    printf("Example :\n");
    printf( "%s -o 1 use a %s OLED\n\n", name, oled_type_str[1]);
    printf( "%s -o 4 -v use a %s OLED being verbose\n", name, oled_type_str[4]);
}


/* ======================================================================
Function: parse_args
Purpose : parse argument passed to the program
Input 	: -
Output	: -
Comments: 
====================================================================== */
void parse_args(int argc, char *argv[])
{
    static struct option longOptions[] =
    {
        {"oled"	  , required_argument,0, 'o'},
        {"verbose", no_argument,	  	0, 'v'},
        {"help"		, no_argument, 			0, 'h'},
        {0, 0, 0, 0}
    };

    int optionIndex = 0;
    int c;

    while (1) 
    {
        /* no default error messages printed. */
        opterr = 0;

        c = getopt_long(argc, argv, "vho:", longOptions, &optionIndex);

        if (c < 0)
            break;

        switch (c) 
        {
            case 'v': opts.verbose = true	;	break;

            case 'o':
                      opts.oled = (int) atoi(optarg);

                      if (opts.oled < 0 || opts.oled >= OLED_LAST_OLED )
                      {
                          fprintf(stderr, "--oled %d ignored must be 0 to %d.\n", opts.oled, OLED_LAST_OLED-1);
                          fprintf(stderr, "--oled set to 0 now\n");
                          opts.oled = 0;
                      }
                      break;

            case 'h':
                      usage(argv[0]);
                      exit(EXIT_SUCCESS);
                      break;

            case '?':
            default:
                      fprintf(stderr, "Unrecognized option.\n");
                      fprintf(stderr, "Run with '--help'.\n");
                      exit(EXIT_FAILURE);
        }
    } /* while */

    if (opts.verbose)
    {
        printf("%s v%s\n", PRG_NAME, PRG_VERSION);
        printf("-- OLED params -- \n");
        printf("Oled is    : %s\n", oled_type_str[opts.oled]);
        printf("-- Other Stuff -- \n");
        printf("verbose is : %s\n", opts.verbose? "yes" : "no");
        printf("\n");
    }	
}


/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments: 
====================================================================== */
int main(int argc, char **argv)
{
    int i;

    // Oled supported display in ArduiPi_SSD1306.h
    // Get OLED type
    parse_args(argc, argv);

    // SPI
    if (display.oled_is_spi_proto(opts.oled))
    {
        // SPI change parameters to fit to your LCD
        if ( !display.init(OLED_SPI_DC,OLED_SPI_RESET,OLED_SPI_CS, opts.oled) )
            exit(EXIT_FAILURE);
    }
    else
    {
        // I2C change parameters to fit to your LCD
        if ( !display.init(OLED_I2C_RESET,opts.oled) )
            exit(EXIT_FAILURE);
    }

    display.begin();

    // init done
    display.clearDisplay();   // clears the screen  buffer
    display.display();   		// display it (clear display)

    if (opts.oled == 5)
    {
        // showing on this display is very slow (the driver need to be optimized)
        sleep_divisor = 4;

        for(char i=0; i < 12 ; i++)
        {
            display.setSeedTextXY(i,0);  //set Cursor to ith line, 0th column
            display.setGrayLevel(i); //Set Grayscale level. Any number between 0 - 15.
            display.putSeedString("Hello World"); //Print Hello World
        }

        sleep(2);

    }

    display.clearDisplay();
    // text display tests
    display.setTextSize(1);
    display.setTextColor(BLACK,WHITE);
    display.setCursor(0,0);
    display.print("=Tor=");
    display.setTextSize(1);
    display.setCursor(36,0);
    display.setTextColor(WHITE);
    display.printf("Relay v%s", PRG_VERSION);
    display.display();
    scrollText();
    // Free PI GPIO ports
    display.close();

}


