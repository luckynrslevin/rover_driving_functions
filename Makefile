TARGET = drivingcontrol

$(TARGET) : drivingcontrol.c 
	$(CC) drivingcontrol.c -o $(TARGET) -l bcm2835

clean:
	rm -f $(TARGET)
