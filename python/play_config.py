import ConfigParser
config_path = 'car.ini'

config = ConfigParser.ConfigParser()
with open(config_path, 'rwb+') as configfile:
    config.readfp(configfile)
    
config.set('calibration','center_steering_us',1470)

with open(config_path, 'rwb+') as configfile:
    config.write(configfile)
