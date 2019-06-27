PIGRRLD_DIR="/usr/local/bin/pigrrld"
RC_LOCAL="/etc/rc.local"
PIGRRL_CONFIG="/etc/pigrrld/calibration.dat"

make clean
make all
./bin/calibrator
chmod 666 $PIGRRL_CONFIG
# ./bin/tester
cp ./bin/pigrrld /usr/local/bin/pigrrld
grep -qF -- "$PIGRRLD_DIR" "$RC_LOCAL" || sed -i "/^exit 0/i$PIGRRLD_DIR &" $RC_LOCAL
/usr/local/bin/pigrrld &
