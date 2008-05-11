# Application specific configuration file
# All parameters required by library code should be defined here

# config.inc.template will be copied to your node folder as config.inc at compile
# and then you may edit config.inc in your node folder (personal/nodeXYZ/) for settings

# -- Application specific
TIMER_NUM_TIMERS=3
TIMER_USE_TIMER=2
TIMER_TICKFREQ=1000
APP_HEARTBEAT_TIMER=0
STDCAN_HEARTBEAT_PERIOD=10000


# -- Modules specific
NUMBEROFMODULES=1

## -- sns_ds18x20 specific
#OW_PIN=PD6
#OW_IN=PIND
#OW_OUT=PORTD
#OW_DDR=DDRD

sns_ds18x20_ID=1
sns_ds18x20_SEND_PERIOD=20000
sns_ds18x20_SEND_DELAY=500
sns_ds18x20_SEARCH_TIMEOUT=5
sns_ds18x20_TIMER=1