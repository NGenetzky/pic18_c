#ifndef RINGBUFFER_GENETZKY_H
#define	RINGBUFFER_GENETZKY_H

    int ringbuffer_is_empty();
    void ringbuffer_clear();
    void ringbuffer_put(unsigned data);
    int ringbuffer_get();
    void ringbuffer_publish();
    void ringbuffer_to_i2c();

#endif //RINGBUFFER_GENETZKY_H