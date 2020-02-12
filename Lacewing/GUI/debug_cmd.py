# File:		debug_cmd.py
# Author: 	Lei Kuang
# Date:		07th Nov 2019
# @ Imperial College London

# Debug Commands for Lacewing Firmware
# STM32F303RE <-> UART2 <-> PC

# ----------------------------------------------------------------
# Import
# ----------------------------------------------------------------
import serial
import serial.tools.list_ports

import time
import struct

# ----------------------------------------------------------------
# User-defined Commands
# ----------------------------------------------------------------
# Function:                             0x0???
# Function + Parameter 1:               0x1??? + 0x0000
# Function + Parameter 1 + Parameter 2: 0x2??? + 0x0000 + 0x0000

# - User-defined Command Table
command_table = \
{
    # ----------------------------------------------------------------
    # - Function only
    "test_serial":                      0x0000,
    "get_version":                      0x0001,
    "get_pwm_out":                      0x0020,
    "get_pwm_in":                       0x0022,
    "get_adc_value":                    0x0060,
    
    "ttn_reset":                        0x00A0,
    "ttn_read_ram":                     0x00A1, 
    "ttn_get_temp":                     0x00A2,

    # ----------------------------------------------------------------
    # - Function + Parameter 1
    "test_func_1":                      0x1000,
    "set_led":                          0x1010,
    "set_pwm_out_period":               0x1020,
    "write_spi":                        0x1030,
    "read_i2c_1":                       0x1040,
    "read_i2c_2":                       0x1041,
    "get_dac_value":                    0x1050, 
    "stop_dac":                         0x1051, 
    
    "ttn_write_ram":                    0x10A0, 
    "ttn_read_ram_with_vs":             0x10A1, 

    # ----------------------------------------------------------------
    # - Function + Parameter 1 + Parameter 2
    "test_func_2":                      0x2000,
    "write_i2c_1":                      0x2040, 
    "write_i2c_2":                      0x2041,
    "set_dac_value":                    0x2050, 

    "ttn_set_rc":                       0x20A0, 

    # ----------------------------------------------------------------
    # - System Commands
    "quit":                             0x0F00,     # System cmd: Quit the command interface
    "list_serial":                      0x0F01,     # System cmd: List serial port
    "close_serial":                     0x0F02,     # System cmd: Close current serial port
    "open_serial":                      0x1F01,     # System cmd: Open serial port COM [par1]
}

# - User Command Interface
class Debug_Command(object):

    def __init__(self):
        self.serial_obj         = None
        self.serial_port        = None  # COM Port
        self.serial_connected   = False
        self.console_s          = None  # pyQT5 Signal
        
    def is_connected(self):
        return self.serial_connected
  
    def print(self, message):
        if(self.console_s == None):
            print(message,  end='')
        else:
            self.console_s.emit(message)
            
    def set_console(self, console_t):
        self.console_s = console_t
    
    # List Serial Ports
    def list_serial(self, show=False):
        port_list = []
        ports = serial.tools.list_ports.comports()
        
        for port in ports:
            description = port.description
            if (show):
                self.print(description+'\n')
            start = description.index('(') + 1
            end   = description.index(')', start)
            port_list.append(description[start:end])
        port_list.sort()
        return port_list

    # Connect to Serial Port
    def open_serial(self, port='COM6'):
        self.serial_port = port
        self.print('S: Open serial port %s, ' % self.serial_port)
        try:
            self.serial_obj = serial.Serial(self.serial_port, baudrate = 115200)
            self.print('Successful\n')
            self.serial_connected = True
        except:
            self.print('Failed\n')
            
    def close_serial(self):
        self.print('S: Close serial port %s, ' % self.serial_port)
        if(self.serial_connected):
            self.serial_obj.close()
            self.print('Successful\n')
            self.serial_connected = False
        else:
            self.print('No serial port connected\n')

    def decode_cmd(self, input_cmd):
        cmd = input_cmd.split(' ')
        # Decode function
        try:
            func = command_table[cmd[0]]
        except:
            self.print("S: No such command '%s'\n" % cmd[0])
            return None
        n_para = func>>12
        if(n_para==0):
            return (func, None, None)
        # Decode parameter 1
        try:
            par1 = int(cmd[1], 0)
        except ValueError:
            self.print("S: Invalid parameter '%s', must be a number\n" % cmd[1])
            return None
        except IndexError:
            self.print("S: Command '%s' requires %s parameter(s)\n" % (cmd[0], 'a' if n_para==1 else "two"))
            return None
        if(n_para == 1):
            return (func, par1, None)
        # Decode parameter 2
        try:
            par2 = int(cmd[2], 0)
        except ValueError:
            self.print("S: Invalid parameter '%s', must be a number\n" % cmd[2])
            return None
        except IndexError:
            self.print("S: Command '%s' requires two parameters\n" % cmd[0])
            return None
        return (func, par1, par2)

    # Execute Decoded Command
    def execute_cmd(self, input_cmd):
        cmd_decoded = self.decode_cmd(input_cmd)
        if(cmd_decoded==None):
            return None
        else:
            (func, par1, par2) = cmd_decoded

        # System command
        if((func>>8)&0x0F == 0x0F):
            if(func==0x0F00 and self.console_obj==None):
                cmd = input("Are you sure to quit ? y/n: ")
                if(cmd.lower()=='y'):
                    quit(0)
            if(func==0x0F01):
                self.list_serial(show=True)
            if(func==0x0F02):
                self.close_serial()
            if(func==0x1F01):
                self.open_serial('COM' + str(par1))
            return None

        # MCU Command
        # - Check connectivity
        if(self.serial_connected!=True):
            self.print('S: Serial port not connected\n')
            return None
        # - Encode to binary
        cmd_send = struct.pack(">H", func)
        cmd_send = cmd_send + struct.pack(">H", par1 if par1!=None else 0x00)
        cmd_send = cmd_send + struct.pack(">H", par2 if par2!=None else 0x00)
        # - Send encoded command to MCU
        #print(cmd_send)
        self.serial_obj.write(cmd_send)

        # Read Response v1...Not efficient...
        rdata_all = ''
        timeout_cnt = 0
        # - Check if end of message else dynamically print
        while(True):
            # - Check if data available
            numBytes = self.serial_obj.inWaiting()
            if(numBytes!=0):
                timeout_cnt = 0
                # - Read a number of bytes
                rdata = self.serial_obj.read(numBytes)
                # - Decode as utf-8
                rdata = str(rdata, encoding = 'utf-8')
                # - Append characters
                rdata_all = rdata_all + rdata
                # - Check if end flag
                if(rdata_all.find("$")!=-1):
                    break
                else:
                    self.print(rdata_all)
                    rdata_all = ''
            # - Check if timeout
            else:
                timeout_cnt = timeout_cnt + 1
                if(timeout_cnt==100000):
                    raise TimeoutError 

        # - Print remaining messgaes
        endflag = rdata_all.find("$")
        if(rdata_all[ : endflag]!=''):
            self.print(rdata_all[ : endflag])
        rdata_all = rdata_all[endflag : ]

        # - Check if end of transmission
        while(rdata_all[-1]!="#"):
            numBytes = self.serial_obj.inWaiting()
            if(numBytes!=0):
                rdata = self.serial_obj.read(numBytes)
                rdata = str(rdata, encoding = 'utf-8')
                rdata_all = rdata_all + rdata

        # - Decode returned data
        if(rdata_all[1]!="["):
            return int(rdata_all[1:-1]) if rdata_all!='$#' else None
        else:
            # "[1,2,3]" -> [1,2,3]
            dict = {'dataList': []}
            exec('dataList=%s' % rdata_all[1:-1],  dict)
            return dict['dataList']

if __name__ == "__main__":
    mcu_debug = Debug_Command()
    mcu_debug.print("\n")
    mcu_debug.print("---- Command Line Interface for Lacewing Debug ----\n")
    mcu_debug.print("---- S: Message from system\n")
    mcu_debug.print("---- M: Message from microcontroller\n")
    mcu_debug.open_serial('COM6')
    mcu_debug.print("S: type your command, e.g., 'test'\n")
    while(True):
        time.sleep(0.01)
        input_cmd = input("")
        try:
            res = mcu_debug.execute_cmd(input_cmd.lower())
        except TimeoutError:
            mcu_debug.print("S: Timeout.\n")
