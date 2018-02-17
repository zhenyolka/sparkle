#include "sound_sles.h"
#include "were/were_event_loop.h"
#include "common/sparkle_connection.h"
#include "common/sparkle_server.h"
#include <SLES/OpenSLES.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

//==================================================================================================

SoundSLES::~SoundSLES()
{
    state = SL_PLAYSTATE_STOPPED;
    SLresult result = (*playerPlay)->SetPlayState(playerPlay, state);
    checkResult(result);
    result = (*playerBufferqueue)->Clear(playerBufferqueue);
    checkResult(result);
    clearQueue();
    
    (*playerObject)->Destroy(playerObject);
    (*outputmixObject)->Destroy(outputmixObject);
    (*engineObject)->Destroy(engineObject);
    
    delete _server;
}

SoundSLES::SoundSLES(WereEventLoop *loop)
{
    _loop = loop;
    
    _server = new SparkleServer(_loop, "/dev/shm/sparkle-sound.socket");
    _server->signal_packet.connect(_loop, std::bind(&SoundSLES::packet, this, _1, _2));

    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    checkResult(result);
    
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    checkResult(result);
    
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    checkResult(result);

    // create output mix
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputmixObject, 0, NULL, NULL);
    checkResult(result);
    
    result = (*outputmixObject)->Realize(outputmixObject, SL_BOOLEAN_FALSE);
    checkResult(result);

    // create audio player

    locator_bufferqueue.locatorType = DATALOCATOR_BUFFERQUEUE;
    locator_bufferqueue.numBuffers = 255;
    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix = outputmixObject;

    pcm.formatType = SL_DATAFORMAT_PCM;
    pcm.numChannels = 2;
    pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
    pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm.containerSize = 16;
    pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audiosrc.pLocator = &locator_bufferqueue;
    audiosrc.pFormat = &pcm;
    audiosnk.pLocator = &locator_outputmix;
    audiosnk.pFormat = NULL;


    SLInterfaceID ids[1] = {IID_BUFFERQUEUE};
    SLboolean flags[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audiosrc, &audiosnk, 1, ids, flags);
    checkResult(result);
    
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    checkResult(result);

    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    checkResult(result);
    
    result = (*playerObject)->GetInterface(playerObject, IID_BUFFERQUEUE, &playerBufferqueue);
    checkResult(result);

    result = (*playerBufferqueue)->RegisterCallback(playerBufferqueue, callback, this);
    checkResult(result);
    
    busy = false;
}

void SoundSLES::checkQueue()
{
    if (busy)
        return;
    
    if (_queue.size() == 0)
        return;
    
    SparklePacket *packet = &_queue.front();
    unsigned int size = packet->size() - packet->readPosition();
    SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, packet->getData(size), size);
    checkResult(result);

    busy = true;
}

void SoundSLES::clearQueue()
{
    _queue.clear();
}

void SoundSLES::callback(BufferQueueItf playerBufferqueue, void *data)
{
    SoundSLES *sound = reinterpret_cast<SoundSLES *>(data);
    
    sound->_queue.pop_front();
    
    sound->busy = false;
    
    sound->checkQueue();
}

//==================================================================================================

void SoundSLES::packet(SparkleConnection *client, SparklePacket packet)
{
    uint32_t operation = packet.getUint32();
        
    if (operation == 0)
    {
        _queue.push_back(packet);
        checkQueue();

    }
    else if (operation == 1)
    {
        state = SL_PLAYSTATE_PLAYING;
        SLresult result = (*playerPlay)->SetPlayState(playerPlay, state);
        checkResult(result);
        checkQueue();
    }
    else if (operation == 2)
    {
        state = SL_PLAYSTATE_STOPPED;
        SLresult result = (*playerPlay)->SetPlayState(playerPlay, state);
        checkResult(result);
        result = (*playerBufferqueue)->Clear(playerBufferqueue);
        checkResult(result);
        clearQueue();
        busy = 0;
    }
}
    
//==================================================================================================

void SoundSLES::beep()
{
    const int SINE_FRAMES = 44100 / 20;

    typedef struct
    {
        short left;
        short right;
    } frame_t;

    unsigned i;
    float pi2 = 3.14 * 2;
    float hz = 441*5;
    float sr = 44100;

	frame_t sine[SINE_FRAMES];

    for (i = 0; i < SINE_FRAMES; ++i) {
        sine[i].left = sin((float) (i  / (sr / hz)) * pi2 ) * 32000.0;
        sine[i].right = sine[i].left;
    }

    frame_t *buffer;
    unsigned size;

    buffer = sine;
    size = sizeof(sine);

    SLresult result = (*playerBufferqueue)->Enqueue(playerBufferqueue, buffer, size);
    checkResult(result);
}

//==================================================================================================

