CLEAN=1
PASS=""
SSID=""
WEBHOOK=""
IMG=""

while [[ $# -gt 0 ]]; do
  case $1 in
    -c|--continue)
      CLEAN=0
      shift
      ;;
    -p|--pass)
      PASS="$2"
      shift
      shift
      ;;
    -s|--ssid)
      SSID="$2"
      shift
      shift
      ;;
    -w|--webhook)
      WEBHOOK="$2"
      shift
      shift
      ;;
    -i|--img)
      IMG="$2"
      shift
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

if [[ $CLEAN -eq 1 ]]
then
    if [ -d build ]
    then
    	    rm -rf build
    fi
    mkdir build
fi

cmake -D PICO_MBEDTLS_PATH=/home/jerboa/pico-sdk/lib/mbedtls/ -D WIFI_SSID=$SSID -D WIFI_PASSWORD=$PASS -D WEBHOOK=$WEBHOOK -D IMG=$IMG -B build -S .
(cd build && make)
