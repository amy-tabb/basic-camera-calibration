FROM amytabb/ubuntu18-ceres-opencv4-contrib-all:latest

MAINTAINER Amy Tabb

#cass
WORKDIR /installed_libs/

RUN git clone https://github.com/amy-tabb/basic-camera-calibration.git

WORKDIR /installed_libs/basic-camera-calibration/build/

RUN cmake ../src

RUN make

RUN make install

WORKDIR /host_dir
