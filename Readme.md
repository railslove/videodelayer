# Bufferlulz

Buffers the shit out of your piped stuff.

    $ gcc timed_buffer.c
    $ ./a.out $DELAY

For example with ffmpeg and webcam 0:

    $ ffmpeg -f qtkit -i "0" -s 1280x720 -b 3072k -framerate 30 -threads 0 -c:v libx264 -preset ultrafast -tune zerolatency -f mpegts - | ./a.out 15

Send a USR1-Signal to the a.out process in order to trigger video capturing for $DELAY:

    $ ps ax | grep "a.out" | grep -v "Chrome" | grep -v grep | awk '{print "kill -USR1 " $1}' | sh
