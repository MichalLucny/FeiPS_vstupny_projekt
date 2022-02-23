import os
import ctypes

# made on python 3.10

def check_header(byte_1, byte_2, byte_3):
    if byte_1 == 0xAA and byte_2 == 0xBB and byte_3 == 0x01:
        return 0
    else:
        return 1

def check_byte1(input_byte, output_byte):
    if bin(input_byte).count('1') == output_byte:
        return 0
    else:
        return 1

def check_byte23(input_byte_1, input_byte_2, output_byte_1, output_byte_2):
    # a bit of a shortcut here
    if input_byte_2 == output_byte_2 and input_byte_1 != output_byte_1:
        return 0
    else:
        return 1
    
def crc16(data: str, poly: hex = 0xA001) -> str:
    # Copyright (c) 2021 Jordan Kalebu
    
    # Permission is hereby granted, free of charge, to any person obtaining a copy
    # of this software and associated documentation files (the "Software"), to deal
    # in the Software without restriction, including without limitation the rights
    # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    # copies of the Software, and to permit persons to whom the Software is
    # furnished to do so, subject to the following conditions:

    # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    # SOFTWARE.
    
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc >> 1) ^ poly
                   if (crc & 0x0001)
                   else crc >> 1)

    hv = hex(crc).upper()[2:]
    blueprint = '0000'
    return (blueprint if len(hv) == 0 else blueprint[:-len(hv)] + hv)
         
def check_crc(data_array, crc_array, flag):
    
    crc = int(crc16(data_array), base=16)
   
    # this looks strange at the first glance but is right
    if flag == "amd64":
        crc_bytes = crc.to_bytes(2,'big')
    else:
        crc_bytes = crc.to_bytes(2,'little')
   
    if crc_bytes[0] == crc_array[0] and crc_bytes[1] == crc_array[1]:
        return 0
    else:
        return 1




# change this line to test other flag 
# flags are "amd64" and "armv7E"
flag = "amd64"

result = os.system(".\\x64\\Debug\\FeiPS_vstupny_projekt.exe " + flag)
if result>0:
  print("PythonScript: Generator of output.dat failed to run properly")
  quit()

try:
  f = open("output.dat","rb")
except IOError:
  print ("PythonScript: Could not open file!")
  quit()  

# negative errCount means empty file
# positive non-zero errCount means errors in output
errCount=-1; 

while True:
  data = bytearray()
  data=f.read(9)

  
  
  # checking for EOF
  # header bytes are the ones that have to be with a nonzero value
  if len(data)<9:
      break
  elif errCount == -1:
      errCount = 0

  errCount = errCount + check_header(data[0],data[1],data[2])\
                      + check_byte1(data[3],data[4])\
                      + check_byte23(data[5],data[6],data[7],data[8])

  # honestly I don't know why two 0xcc bytes appear here and there
  # feel free to explain it in person
  datacc1 = bytearray()
  datacc1=f.read(2)

  datacrc = bytearray()
  datacrc=f.read(2)

  errCount = errCount + check_crc(data,datacrc,flag)
  
  datacc2 = bytearray()
  datacc2=f.read(2)

if errCount==0:
    print("OK")
else:
    print("CHYBA")

f.close()

