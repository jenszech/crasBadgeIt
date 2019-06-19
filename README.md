# crasBadgeIt
ePaper interface for Conference Room Assistent System (CRAS)

Note that because this example puts the ESP into a deep sleep if you want to reprogram it via the USB then you have to manually stop and restart it using the slider switch at the moment just prior to the upload starting.

Das Projekt setzt sich dabei aus mehreren Modulen zusammen:
* **[cras](https://github.com/jenszech/cras)** - stellt das benötigte Backend bereit
* **[kras](https://github.com/julienP88/kras)** - stellt ein responsives Angular Frontend bereit
* Optional **[crasBadgeIt](https://github.com/jenszech/crasBadgeIt)** - Ein microcontroller projekt for small ePaper devices
* Optional **[crasAndroidViewer](https://github.com/jenszech/crasAndroidViewer)** - Eine Android App mit einem einfachem fullscreen Webviewer (benötigt das kras Frontend)

## Features

* tbd.

### Roadmap

* tbd.

## Getting Started

### Prerequisites
- See the base required libraries in the root Readme for this repository
- GxEPD 2.x
- ArduinJSON 5.x
- Time 1.5.0 - You can find the repo [here] (https://github.com/PaulStoffregen/Time)).  

### Instructions
- Download code sample and required libraries, and compile/upload in the Arduino IDE (1.8.5)
- Sometimes the API will return an error if the service is unavailable, simply restart Badgy to try again


## Built With

* tbd.

## Contributing

Please read [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Jens Zech** - *Initial work* - [GitHub Profil](https://github.com/jenszech)
* **fadoscha** - *Team Member* - [GitHub Profil](https://github.com/fadoscha)
* **s-wolff** - *Team Member* - [GitHub Profil](https://github.com/s-wolff)

See also the list of [contributors](https://github.com/jenszech/crasBadgeIt/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
