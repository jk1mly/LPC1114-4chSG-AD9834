import serial
inst=serial.Serial("COM12",115200)
Freq=[1000,1000,1000,1000]  ##kHz unit
Pha=[0,0,0,0]               ##deg. unit
Ampl=[1000,1000,1000,1000]  ##mV unit. 50ohm loaded.

buf=f'{Freq[0]*1000:08}'+f'{Pha[0]:03}'+f'{Ampl[0]:04}'+f'{Freq[1]*1000:08}'+f'{Pha[1]:03}'+f'{Ampl[1]:04}'+f'{Freq[2]*1000:08}'+f'{Pha[2]:03}'+f'{Ampl[2]:04}'+f'{Freq[3]*1000:08}'+f'{Pha[3]:03}'+f'{Ampl[3]:04}'
inst.write(buf.encode())
print('Ch1 Freq=',Freq[0],'kHz, Pha=',Pha[0],'deg., Ampl=',Ampl[0],'mV\nCh2 Freq=',Freq[1],'kHz, Pha=',Pha[1],'deg., Ampl=',Ampl[1],'mV\nCh3 Freq=',Freq[2],'kHz, Pha=',Pha[2],'deg., Ampl=',Ampl[2],'mV\nCh4 Freq=',Freq[3],'kHz, Pha=',Pha[3],'deg., Ampl=',Ampl[3],'mV\n')