/**
* @file appusbsensormain.c
*
* Copyright (c) 2016 Solid-Motion
*
* @brief Main for SGA-USB firmware
*
* @version 7
*
* @mainpage SGA-USB Sensor Interface and Data Converter
*
* @section section_1 intro 
* Smart Sensor Interface and Data Acquisition Solutions
*
* @image html "Solid-Motion Solution.jpg"
*
* @image html Screenshot_HPStream.png
*
* ---
*
* @section section_2 USB interface 
* 	This device implements End Point 0 for USB commands and End Point 1 for Interrupt in
* 	@li USB commands are initiated by the host for EP 0
* 	@li TEDS data is also sent via a HID record in EP 0
* 	@li Measured data is sent in HID records via device-to-host initiated query on EP1
*
* @image html Slide8.jpg
* @image html Slide9.jpg
* @image html USB_Stack.jpg
*
* ---
*
* @section section_3 firmware structure
* @{Has three components:
* 1. Main loop
*	@li Initialization
*	@li Poll USB looking for device requests
* 2. Timer interrupt service that controls ADC sampling rate
* 3. SPI bus interrupt service when ADC has completed a measurement
*	
* <STRONG>Main Loop</STRONG>
*
* Main loop flow chart
*
* @image html "main loop.png"
*
* Main call graph
*
* @image html main.png
*	
* <STRONG>Timer interrupt service</STRONG>
*
* Timer ISR call graph
*
* @image html timer_ISR.png
*
* <STRONG>SPI bus interrupt service</STRONG>
*
* SPI and ADC ISR call graph
*
* @image html spi_ISR.png
*
* ---
*
* @section section_4 directory organization
*
* @image html smfirmware_directory_structure.jpg
*
* @section section_5 control transaction sequence
*
* @image html Setup.jpg
*
* @}
* 
*/

#include "stdafx.h"
#include "serial_debug.h"
#include "usb_stack.h"
#include "usb_stack_std.h"
#include "hid_handler.h"
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <util/delay_basic.h>
#include "usb_settings.h"
#include <teds.h>
#include <util/delay.h>
#include "messages.h"
//#include "appusbsensormain.h"

// clock speed defined in make file (F_CPU 8000000)

ISR(__vector_default)
	{
	debug_write_string("VD!\n");
	}

/********************************* TEDS Management ************************************/
/** @defgroup teds_management TEDS management
* @brief Read TEDS
*
* @{
*/
	
struct
	{
	U8 pending_teds_read;
	report_feature_teds_state_t teds_state;
	U8 teds_data[128]; //may depend on memory architecture, teds_data array size is 4 for DS2431 and 16 for DS2431
	} global_teds;
	
/**Called by the main event loop.  If teds is not ready or the connect
 * state changed, then re-read. All of this is done on every pass through the main loop
 */
void serviceteds_state()  // all of this is done on every pass through the main loop
	{
	if (!global_teds.pending_teds_read)
		{
		return;  // Nothing todo
		}
   
	global_teds.pending_teds_read = 0;
	teds_init();      // Setup teds port
	_delay_ms(2);
   
	// Reset teds state
	global_teds.teds_state.teds_ready = 1;
	global_teds.teds_state.teds_sensor_connected = 0;
	global_teds.teds_state.teds_data_available = 0;

	/** @brief Read the 8 bytes of the TEDS ROM section */
	if (!teds_read_rom33(global_teds.teds_state.teds_rom33)) //first byte is the ROM family code
	                                                         //0x23,"DS2433","4Kb EEPROM memory"
															 //0x2D,"DS2431","1024-bit, 1-Wire EEPROM"
		{
		debug_write_string("NO TEDS(R)\n");
		return;
		}
		
	if (!teds_read_memory(global_teds.teds_data, sizeof(global_teds.teds_data)))
		{
		debug_write_string("NO TEDS(M)\n");
		return;
		}
   
	// Got it all
	global_teds.teds_state.teds_sensor_connected = 1;
	global_teds.teds_state.teds_data_available = 1;
	debug_write_string("TEDS READ\n");
	}
	
/**
 @}
 */
 
/********************************* HID Report Response Functions **********************/
/** @defgroup hid_request HID TEDS report processing
* @brief USB class requests 
*
*  All of the reports are returned when the host issues a GET REPORT DESCRIPTOR
* - Report ID 1 REPORT_FEATURE_QUERY_DEVICE_STATE
* - Report ID 2 REPORT_FEATURE_QUERYTEDS_STATE
* - Report ID 10 REPORT_FEATURE_QUERYTEDS_PAGE_0
* - Report ID 11 REPORT_FEATURE_QUERYTEDS_PAGE_1
* - Report ID 12 REPORT_FEATURE_QUERYTEDS_PAGE_2
* - Report ID 13 REPORT_FEATURE_QUERYTEDS_PAGE_3
* - Report ID 30 REPORT_OUT_CMD_RESETTEDS
*
* @{
*/
U8 get_report_FEATURE_QUERYTEDS_STATE(U8 length)
	{
	U8 i;
	
	// Complete sequence for Control Read Transfer
	Usb_ack_receive_setup(); //Setup transaction, handshake stage ACK
	
	Usb_write_byte(REPORT_IDTEDS_STATE); // Write reportId
	
	for (i = 0; i < length - 1; i++) // Shorten length by one since we already wrote out the reportId
		{
		Usb_write_byte(((U8*)&global_teds.teds_state)[i]);
		}
   
	Usb_send_control_in(); // IN data transaction, data stage, done sending data
	
	while (!Is_usb_receive_out()); // Tests if OUT received; SIE command
	
	Usb_ack_receive_out(); // Status transaction, handshake stage ACK
	
	return 1;
	}
	
	U8 get_report_FEATURE_QUERYTEDS_PAGE(U8 pageNumber, U8 length)
	{
	// Complete sequence for Control Read Transfer
	
	U8 *page = global_teds.teds_data + pageNumber * 32;
	U8 i;
	
	Usb_ack_receive_setup(); //Setup transaction, handshake stage ACK
	Usb_write_byte(10 + pageNumber); // Write report id if it's the first packet.
	
	for (i = 0; i < length - 1; i++) // Shorten length by one since we already wrote out the reportId
	{
		Usb_write_byte(*page);
		page++;
	}
   
	Usb_send_control_in(); // IN data transaction, data stage, done sending data
	while (!Is_usb_receive_out()); // Tests if OUT received; SIE command
	Usb_ack_receive_out(); // Status transaction, handshake stage ACK
   
	return 1;
	}
	
U8 set_report_FEATURE_QUERYTEDS_PAGE(U8 pageNumber, U8 length)    //write 32 byte pages to teds memory one at a time
	{															   //data sent from device includes the pageNumber; collect 0-32 in array, write 1-32 to TEDS
		
	global_teds.pending_teds_read = 1; //needed?

	// Complete sequence for Control Write Transfer & get the data
	usb_stack_read_control_write(global_teds.teds_data, length); 
	
	global_teds.teds_state.teds_status = 0; // Reset teds status
	
	// Transfer global_teds.teds_data array to TEDS memory page by page
	// bLength is only 32 for memory write; don't use HID ReportID
	// Returns status byte
	global_teds.teds_state.teds_status = (teds_write_memory(global_teds.teds_data, pageNumber, length - 1)); 
	
	return 1;
	}	

U8 set_report_cmd_reset()  //Reset TEDS
	{
	global_teds.pending_teds_read = 1;
	usb_stack_read_control_write(0, 0); // Complete sequence for Control Write Transfer; params: global_teds.teds_data, pageNumber, length
	usb_stack_ack_control_write(1); // Partial sequence for Control Read Transfer;  I don't understand why this sequence is needed, but it is.
									// Calling usb_stack_ack_control_write(U8 success) should be preceded by Usb_ack_receive_setup()
	return 1;
	}

// Process HID reports
// usb_stack_hid_user_set_report prototype in hid_handler.h
U8 usb_stack_hid_user_set_report(U8 reportType, U8 reportId, U8 length)
	{
   
	if (reportId == REPORT_OUT_CMD_RESET_TEDS)
	{
		return set_report_cmd_reset();
	}
	else 
	if (reportId >= REPORT_FEATURE_QUERYTEDS_PAGE_0 && reportId <= REPORT_FEATURE_QUERYTEDS_PAGE_3)  //Somehow 14 gets by here
	{
		return set_report_FEATURE_QUERYTEDS_PAGE((reportId - 10), length); //convert reportID to pageNumber
	}
	else
		if (reportId == REPORT_OUT_CMD_DEVICE_RESET)  
			{
				return set_report_REPORT_OUT_CMD_DEVICE_RESET(); 
			}
	return 0;
	}
	
// usb_stack_hid_user_get_report prototype in hid_handler.h
U8 usb_stack_hid_user_get_report(U8 reportType, U8 reportId, U8 length)
	{
   
	if (reportType == REPORTTYPE_FEATURE)
		{
		if (reportId >= REPORT_FEATURE_QUERYTEDS_PAGE_0 && reportId <= REPORT_FEATURE_QUERYTEDS_PAGE_3)
			{
			return get_report_FEATURE_QUERYTEDS_PAGE(reportId - REPORT_FEATURE_QUERYTEDS_PAGE_0, length);
			}
      
		// Feature report
		if (reportId == REPORT_FEATURE_QUERYTEDS_STATE)
			{
			return get_report_FEATURE_QUERYTEDS_STATE(length);
			}
		}
	return 0;
	}
/**
@}
*/

/********************************* Power On Management ********************************/
/** @defgroup power Power-on management
* @brief Power-on management
*
* @{
*/

/** We are imposing very long delays here for debugging so that analog
* power does not get turned on until well after enumeration.  For production,
* this should be tuned way down.
*/
#define DEFAULT_POWERONTICKS 100 //was 10000
   
/** If this value is >0, then it will be decremented each time through the
* main loop.  When it reaches zero, the variable global_state_poweron_stable
* is set to 1, which indicates that the requisite amount of time has passed
* after usb configuration and "dangerous" things can be done safely.
*/
U32 global_state_poweronticks;

/** If 0, then dangerous things should be avoided because we are still either
* not configured or in the immediate period following configuration.
*/
   
U8 global_state_poweron_stable;

/** Called when the device is configured.  Starts the poweron ticks
 * timer.
 * This function is called from the usb stack (see globals.h)
 !*/
void appusbsensor_set_configuration()
	{
	if (!global_state_poweron_stable)
		{
		// Start the counter for actual power-up
		global_state_poweronticks=DEFAULT_POWERONTICKS;
		}
	}

/*! Called by the main loop when a requisite amount of time has passed
 * after configuration and it is ok to power on sub-systems.  This will
 * only be called once when the global_state_poweron_stable variable
 * transitions to 1.
 !*/
void appusbsensor_poweron_stable()
	{
	debug_write_string("PWRON\n");
	PORTA|=(1<<PA0); //Turn on 5.1V analog supply
	PORTE|=(0<<PE7); //Turn off yellow LED   //////////////////////////////////////////////////////////////////////////
	PORTE|=(1<<PE6); //Turn on green LED   //////////////////////////////////////////////////////////////////////////
	
    global_teds.pending_teds_read=1; // Request teds read
	}
	
//! @}
/*** End Poweron Management ***/

/********************************* ADC Config *****************************************/
#define ADC_CHANNEL 0x0
#define ADC_CONFIG  0x800 // Internal(4V) ref, Short sample, internal clock, Single Shot

/********************************* SPI/ADC Management *********************************/
U16 adc_accum;
U8 adc_state;
#define ADC_STATE_IDLE 0x10
#define ADC_STATE_PGM  0x20
   
U16 adc_spi_out;  // Word to be sent out
U16 adc_spi_in;   // Word to be sent in
U8  adc_spi_count;// Number of bytes remaining to transfer

/********************************* TIMER Type Defs & Setup***********************************/
// Calibrate timer values.  These are based on the system clock speed/256.
const U16 TIMER_COMPARE_VALUES[]={31,31,31,32}; // 1kHz @ 8MHz (exact)
//const U16 TIMER_COMPARE_VALUES[]={31250}; // 1s period @ 8MHz/256
//const U16 TIMER_COMPARE_VALUES[]={0xffff}; // max interval
   
const U8   TIMER_COMPARE_LENGTH=sizeof(TIMER_COMPARE_VALUES)/sizeof(TIMER_COMPARE_VALUES[0]);
U8 timer_compare_index;
U16 timer_last_compare;
   
/* Running timer sequence */
U32 timer_value;
   
/********************************* Sample Accumulator *********************************/
//! @defgroup sample ADC sample accumulator
//! @brief  ADC sample accumulator
//! @{
#define SAMPLE_MAX_COUNT 25
U8 samples_per_packet=SAMPLE_MAX_COUNT;
   
//! @brief struct that sets up the columns of the eventual complete data structure for EP1 data
struct sample_packet
	{
	/* Control/ID */
	U8 control;  //always set to 0xFF. Could be used for the Record ID
      
	/* Number of samples */
	U8 sample_count;  //Number of measurements in the packet. Should be 25.
	
	/*Serial packet number - Overflow on full.  Only useful for relative checks */
	U16 packet_number; //This is the sequential number associated with each EP1
                            //read starting when the board is reset. It will roll-over every
                            //0xFFFF reads
      
	/* Number of timer ticks since last packet */
	U32 packet_deltaticks;
      
	/* Sample data */
	U16 samples[SAMPLE_MAX_COUNT];  //array of the averaged ADC samples
	};
   
U16 last_packet_number;
S8 last_packetticks;

struct sample_packet sample_packets[2];   //sets up an array of two structs for EP1 data. I don't know why two structs in an array are needed

// Synchronization flags
U8 lock_sample_packet_index = 0xff;
U8 can_read_sample_packet_index = 0xff;
U8 write_sample_packet_index = 0x00;
//! @}

/********************************* HID Report Management ******************************/
//! @defgroup hid HID ADC processing
//! @brief  HID ADC processing
//! @{
#define REPORTTYPE_SAMPLES 0
#define REPORTTYPE_DEFAULT REPORTTYPE_SAMPLES
   
U8 requested_reporttype = REPORTTYPE_SAMPLES;
   
void service_hid_send_samples_report()
	{
	U8 sreg = SREG;
	U8 index;
	struct sample_packet *packet; //pointer to struct
	U8 packet_size;
	U8 i;
      
     // Disable ints and lock a packet for read if available
    cli();
    index = can_read_sample_packet_index;
    if (index == 0xff)
		{
         // No packet available
         SREG = sreg;
		 //debug_write_string("_Hid");// exiting the function here
         return;
		}
		
	lock_sample_packet_index = index;
    can_read_sample_packet_index = 0xff;  // Clear so we don't read this one again

      // Restore ints
      SREG =sreg;
      
	// Send the packet data; not clear what the packet_size calculation is.
	packet = sample_packets + index;
	packet_size = sizeof(struct sample_packet) - sizeof(packet -> samples) +  //equivalent to sample_packet.samples
		(packet -> sample_count) * sizeof(packet -> samples[0]); //equivalent to sample_packet.sample_count * sizeof(sample_packet.samples[0]) 
     
	 
	 
	for (i = 0; i < (packet_size + 1); i++)  //packet_size is 57, add 1 for the report id; report id can be moved to struct sample_packets
		{
		if (i == 0) 
		Usb_write_byte(0x01); //write report id if it's the first packet
		Usb_write_byte(((U8*)packet)[i]);
		}
      
	for (; (packet_size - 1) < 63; packet_size++)  //reduce packet size by one since the terminal go-around in the previous loop incremented packet_size before exiting the loop
		{
		Usb_write_byte(0x0);
		}
      //debug_write_string("HID ");
	Usb_ack_in_ready();

	//debug_write_string("UPK");
	//debug_write_hex(packet_size);
	//debug_write_hex(LSB(packet->packet_number));
	//debug_write_nl();
      
	// Finally, unlock the packet
	lock_sample_packet_index = 0xff;
	}
   
void service_hid_interrupt_endpoint()
	{
	if (!usb_stack_is_enumerated())
	{
	return;
	}
	Usb_select_endpoint(EP_HID_IN);
	//if (Is_usb_write_enabled()) //tests if endpoint write is allowed - this is resetting RWAL in UEINTX so commenting out
		//{
		if (requested_reporttype == REPORTTYPE_SAMPLES) //requested_reporttype is hard coded to REPORTTYPE_SAMPLES so this test isn't needed
			{
			service_hid_send_samples_report();
			}
		//}
		else
		{
			//debug_write_string("IUWE!\n");
		}
	}

   
/********************************* Accumulate packets *********************************/
inline void flip_sample_packet(struct sample_packet *cur, U8 cur_index)
	{
	U8 sreg;
	U32 curtimer_value;
      
	// Grab the next packet number
	last_packet_number += 1;
   
	// Disable ints
	sreg = SREG;
	cli();
      
	// Remember the tick count (with ints disabled)
	curtimer_value = timer_value;
      
	if (lock_sample_packet_index != 0xff)
		{
		// There is a read-lock indicating overflow.  Must discard this packet
		cur -> sample_count = 0;  //equivalent to sample_packet.sample_count; "struct name"."column of struct"
         
		// Enable ints and return - next sample will be written to this buffer
		SREG = sreg;
         
		debug_write_string("PKOF!\n");
		return;
		} else
		{
		// There is not a read-lock present.  While ints disabled, clear the
		// can_read_sample_packet value so that a reader will not pick this
		// up while we are finishing it
		can_read_sample_packet_index = 0xff;
         
		// Flip to the next buffer for next sample
		if (cur_index == 0) write_sample_packet_index=1;
		else write_sample_packet_index = 0;
         
		/* Reset so that we start over on the next go round. sample_packets is an array of two structs sample_packet.
		Only two elements in the array so write_sample_packet_index can only be 0 or 1*/
		sample_packets[write_sample_packet_index].sample_count = 0;
         
		// Enable ints
		SREG = sreg;
         
		// Finish this packet
		cur -> packet_deltaticks = curtimer_value - last_packetticks; //LHS equivalent to sample_packet.packet_deltaticks
		last_packetticks = curtimer_value;
		cur -> packet_number = last_packet_number; //LHS equivalent to sample_packet.packet_number
		cur -> control = 0xff;  //LHS equivalent to sample_packet.control; this could be used for the Record ID
         
		// Mark the packet as readable
		can_read_sample_packet_index = cur_index;
		}
	}
   
/* Inlined function for adding to a sample.  Uses a pointer, but
 * inlining should eliminate the pointer access in favor of duplicating
 * the code.
 */
 inline void accum_sample_p(struct sample_packet* cur, U8 cur_index, U16 sample)
	{
	cur -> samples[cur -> sample_count++] = sample;  //LHS equivalent to sample_packet.samples[sample_packet.sample_count++]
	if (cur -> sample_count == SAMPLE_MAX_COUNT)  //LHS equivalent to sample_packet.sample_count
		{
         // Flip buffer
         flip_sample_packet(cur, cur_index);
		}
	}
   
void accum_sample(U16 sample)
	{
	if (write_sample_packet_index == 0)
		{
		accum_sample_p(sample_packets + 0, 0, sample);
		} else
		{
		accum_sample_p(sample_packets + 1, 1, sample);
		}
	}
   //! @}
   
/********************************* ADC & SPI  Management *********************************/
//! @defgroup adc ADC & SPI  Management
//! @brief ADC & SPI  Management
//! @{

//! @brief SPI  assert 
inline void adc_spi_assert()
	{
	PORTB &=~ (1 << PB0);
	}
//! @brief SPI  de-assert  
inline void adc_spi_deassert()
	{
	PORTB |= (1 << PB0);
	}
//! @brief ADC -> SPI  transfer
//! @param word_out ADC result to transfer
void adc_spitransfer(U16 word_out)
	{
	adc_spi_out = word_out;  //command for ADC
	adc_spi_count = 1;
	adc_spi_assert();
	SPDR=MSB(adc_spi_out); //write MSB of word to SPI Data Register (SPDR)
	}
	
//! @brief ADC sample
void adc_sample()
	{
	U16 command = (ADC_CHANNEL << 12); //set input to adc channel 0; TLV2544 data sheet Table 1.
    if (adc_state == ADC_STATE_PGM)
	return;
      
    if (adc_state == ADC_STATE_IDLE)
		{
		adc_accum = 0;
		adc_state = 0x0;
		}
	adc_spitransfer(command); //triggers falling edge of CS/ which starts the conversion
	}
   
//! @brief ADC -> SPI  transfer complete
void adc_spi_xfer_complete()
	{
	U16 sample;
      
	/*debug_write_string("SC");
	debug_write_hex(MSB(adc_spi_in));
	debug_write_hex(LSB(adc_spi_in));
	debug_write_nl();*/
      
	if (adc_state == ADC_STATE_IDLE)
		{
		debug_write_string("SPIIDLE\n");
		return;
		}
      
	if (adc_state == ADC_STATE_PGM)
		{
		adc_state = ADC_STATE_IDLE;
		
		debug_write_string("AP");
		debug_write_hex((ADC_CONFIG & 0xff00) >> 8);
		debug_write_hex(ADC_CONFIG & 0xff);
		debug_write_nl();
		
		adc_spitransfer(0xa000 | ADC_CONFIG); //does this have to be reprogrammed on each adc cycle? 
		return;
		}
      
	// Active state debug
	if (adc_state > 0)
		{
		// Add a sample to the average
		adc_accum += (adc_spi_in >> 4);   // Only 12 upper bits significant
		}
      
	// Initiate next
	if (adc_state == 0x08)
		{
		// Complete
		sample=(adc_accum >> 3);  // Divide by 8
		//debug_write_string("SMP");
		//debug_write_hex(MSB(sample));
		//debug_write_hex(LSB(sample));
		//debug_write_nl();*/
     
         
		adc_accum = 0;  //reset accumulator counter
		adc_state = 0;
         
		accum_sample(sample);
		}
		else
		{
		adc_state += 1;  //not done accumulating samples; increment the sample counter
		}
	}

//! @brief SPI interrupt handling	
ISR(SPI_STC_vect)  //defined in mcu.h
	{
		
	//U8 status=SPSR;
	U8 recv_byte=SPDR;
      
	if (adc_spi_count == 1)
		{
		// MSB transfer just completed and need to do LSB
		MSB(adc_spi_in)=recv_byte;
		SPDR=LSB(adc_spi_out);
		adc_spi_count = 0;
		}
		else
		{
		// LSB transfer just completed and need to signal done
		LSB(adc_spi_in) = recv_byte;
		adc_spi_deassert(); //asserted in Timer Interrupt Service
		adc_spi_xfer_complete();
		}
	}
//! @}

/********************************* Timer Management **************/
//! @defgroup timer Timer management
//! @brief Timer management
//! @{
//!

//! @brief compare timer count to programmed value
inline void adctimer_next_compare()
	{
	U8 sreg;
      
	// Load 16-bit register
	sreg = SREG;
	cli();
      
	timer_last_compare = TIMER_COMPARE_VALUES[timer_compare_index ++];
	OCR1A = timer_last_compare;
	if (timer_compare_index == TIMER_COMPARE_LENGTH) timer_compare_index = 0;
	SREG = sreg;
	}
//! @brief start timer
void adctimer_start()
	{
		
	adctimer_next_compare();
	TIMSK1 = 1 << TOIE1;
	// OCR values are double-buffered.  This will prepare for the next
	// iteration.  Only needed to do twice on init.
	adctimer_next_compare();
	}
	
//! @brief timer interrupt handling	
ISR(TIMER1_OVF_vect)
	{		
	timer_value += timer_last_compare;
	adctimer_next_compare();
	adc_sample();
	adctimer_start();
	}
//! @}

/********************************* ADC Initialization **************/
//! @defgroup adc_initialize Initialize ADC
//! @brief Initialize ADC
//! @{
//!
void adc_initialize()
	{
	//Timer setup
	TCCR1A = 0x3;
	TCCR1B = 0x1c;
	power_spi_enable();
      
/*! ADC sample rate is tied to SPI sclk
	The external ADC chip can work at very high rates, and there
    is no way we could overdrive it, so we will run it at the max rate.
    !*/ 
      
    // Enable SPI - MSTR, Maximum speed, Interrupt enable
    SPCR=(1 << SPE) | (1 << MSTR) | (1 << SPIE);
	SPSR=(1 << SPI2X);  // 2x master speed
      
	debug_write_string("ADCP\n");
	adc_state = ADC_STATE_PGM;
	adc_spitransfer(0xa000);
	}
//! @}

/********************************* Main ***********************************************/
//! @defgroup main Main program loop
//! @brief Main program loop
//! @{

int main(void)
	{
//!- If our reset was caused by the watchdog, go to the watchdog application
	//!
	if (MCUSR & _BV(WDRF))
		{
		// Turn watch dog off
		wdt_reset();
		Wdt_clear_flag();
		Wdt_change_enable();
		Wdt_stop();
		}
	Clear_prescaler();
   
/*!- Impose a delay at startup - reuse the global_state_poweronticks
	variable (just need to make sure it is reset to 0 when done).
	This is kept here to enable debugging by uncommenting it.  
	Slows down the startup sequence.
   !*/
   //!
	for (global_state_poweronticks = 1000000; global_state_poweronticks > 0; global_state_poweronticks--);
	global_state_poweronticks = 0;
   
//!- Config IO Ports
	DDRA=(1 << DDA0) | (1 << DDA1);
	PORTA = 1 << PA1;  // PA0=START=0, PA1=nCSTART=1 (ADC CSTART/)
	DDRE=(1 << DDE7) | (1 << DDE6);
	PORTE|=(1<<PE7); //Turn on yellow LED   
	//!
   
/*!- Initialize analog circuitry at startup.  Note that this was
	added for experimentation.  For some reason, it doesn't mess
    things up if analog circuitry is turned on here, but if done
    any time post startup, it causes a USB disconnect and re-enumerate.
    Comment out this line to see it.  There must be some transients
    in analog initialization that cause the host controller to freak
    out.
    !*/

	// SPI on PORTB
	// MOSI, SCK, SS output.  All others input
	PORTB=(1 << PB0);   // SS=1
	DDRB=(1 << DDB2) | (1 << DDB1) | (1 << DDB0);
   
	//debug_init();
	//debug_write_string("\nAPPPWR\n");

	usb_stack_init_device();
	//debug_write_string("\nstack_init_done\n");
	adc_initialize();
	adctimer_start();

	for (;;) // Main loop
		{
		if (global_state_poweron_stable)
			{
//!- Only do "dangerous" things after power-on state is stable
			serviceteds_state();
			} 
		else
			{
			if (global_state_poweronticks)
				{
				if (--global_state_poweronticks == 0)
					{
					// We're stable now
					global_state_poweron_stable=1;
					// Call the poweron handler
					appusbsensor_poweron_stable();
					}
				}
			}
      
		usb_stack_service();
		service_hid_interrupt_endpoint();
		//debug_service();  //turn on/off "#define USB_DEBUG 1" in stdafx.h
		}
	return 0;
	}
	
	//! @}

