CC = g++
CFLAGS = -I./fftw/include -std=c++20
#LDFLAGS = -L./fftw/lib -lfftw3
LDFLAGS = -L./fftw/lib -Wl,-rpath,./fftw/lib -lfftw3 ##-Wl,-rpath,./fftw/lib は、リンク時に ./fftw/lib をランタイムリンクパスとして埋め込むオプションです。
TARGET = spectrumAnalysis
SRC = spectrumAnalysis.cpp

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
clean:
	rm -f $(TARGET)
