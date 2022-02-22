import os
import ctypes

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
    if input_byte_2 == output_byte_2 and input_byte_1 != input_byte_2:
        return 0
    else:
        return 1

def check_crc(data_array, crc, flag):
    # Copyright (c) 2019 Tiago Ventura
    
    crc = 0xFFFF
    bit = 0

    for i in range(9):
        crc ^= data_array[i]
        for bit in range(8):
            if crc & 0x0001 :
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    
   
    if (flag == "amd64"):
        crc_bytes = crc.to_bytes(2,'little')
    else:
        crc_bytes = crc.to_bytes(2,'big')
   
    if crc_bytes[0] == crc[0] and crc_bytes[1] == crc[1]:
        return 0
    else:
        return 1




# change this line to test other flag 
# flags are "amd64" and "armv7E"
flag = "amd64"

os.system(".\\x64\\Debug\\FeiPS_vstupny_projekt.exe " + flag)

f = open("output.dat","rb")

# negative errCount means empty file
# positive non-zero errCount means errors in output
errCount=-1; 

while True:
  data = bytearray(9)
  for i in range (9):
    data[i]=f.read(1)

  # checking for EOF
  # header bytes are the ones that have to be with a nonzero value
  if not data[0] or not data[1] or not data[2]:
      break
  elif errCount == -1:
      errCount = 0

  errCount = errCount + check_header(data[0],data[1],data[2])\
                      + check_byte1(data[3],data[4])\
                      + check_byte23(data[5],data[6],data[7],data[8])

  # honestly I don't know why two 0xcc bytes appear here and there
  # feel free to explain it in person
  datacc1 = bytearray(2)
  for i in range (2):
    datacc1[i]=f.read(1)

  datacrc = bytearray(2)
  for i in range (2):
    datacrc[i]=f.read(1)

  errCount = errCount + check_crc(data,datacrc,flag)

  datacc2 = bytearray(2)

  for i in range (2):
    datacc2[i]=f.read(1)

if errCount>0:
    print("OK")
else:
    print("CHYBA")


#for i in range (9):
#  print(hex(data[i]))
