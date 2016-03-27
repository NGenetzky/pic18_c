

#define ringbuffer_SIZE 32
static volatile unsigned int ringbuffer_iread = 0;
static volatile unsigned int ringbuffer_iwrite = 0;
static volatile char ringbuffer_array[ringbuffer_SIZE];

int ringbuffer_is_empty(){
    return (ringbuffer_iread == ringbuffer_iwrite);
}

void ringbuffer_clear(){
    ringbuffer_iread = ringbuffer_iwrite;
}

void ringbuffer_put(unsigned data){
    //in your ISR
    ringbuffer_array[ringbuffer_iwrite] = data;
    ringbuffer_iwrite = (ringbuffer_iwrite+1) % ringbuffer_SIZE;
}

int ringbuffer_get(){
    char value;
    if (ringbuffer_iread != ringbuffer_iwrite) {
        value = ringbuffer_array[ringbuffer_iread];
        ringbuffer_iread = (ringbuffer_iread+1) % ringbuffer_SIZE;
        return value;
    } else
        return -1;
}

void ringbuffer_publish(){
    char carray[ringbuffer_SIZE];
    for(int i=0; !ringbuffer_is_empty(); i++){
        carray[i] = ringbuffer_get();
    }

//    Particle.publish("ringbuffer", carray);
}

void ringbuffer_to_i2c(){
//    Wire.beginTransmission(target); // transmit to slave device
//    char data = ringbuffer_get();   
//    Wire.write(data);
//    Wire.endTransmission(target); // transmit to slave device
}
