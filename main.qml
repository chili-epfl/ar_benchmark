import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0
import Tester 1.0
ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Item{
        id:root
        property string test_type: "rotate"
        anchors.fill: parent
        Tester{
            id:tester
            onFinished: status.color="green"
        }


        RowLayout {
            anchors.bottom: buttons.top
            anchors.horizontalCenter: buttons.horizontalCenter
            anchors.leftMargin: 20
            RadioButton {
                text: "Rotate"
                checked: true
                onCheckedChanged: if(checked) root.test_type="rotate"
            }
            RadioButton {
                text: "Shift Color"
                onCheckedChanged: if(checked) root.test_type="shift"

            }
            RadioButton {
                text: "Blur"
                onCheckedChanged: if(checked) root.test_type="blur"
            }
            TextField{
                id:param
                placeholderText: "Test Parameter"
                text: "0"
                validator: DoubleValidator{}
            }
        }
        Row{
            id:buttons
            padding: 10
            anchors.centerIn: parent
            Button{
                text:"Small Dataset"
                onClicked: {
                    status.color="red"
                    tester.run("/home/chili/ar_benchmark/data/small/",root.test_type,parseFloat(param.text))
                }
            }
            Button{
                text:"Medium Dataset"
                onClicked: {
                    status.color="red"
                    tester.run("/home/chili/ar_benchmark/data/medium/",root.test_type,parseFloat(param.text))
                }
            }
            Button{
                text:"Large Dataset"
                onClicked: {
                    status.color="red"
                    tester.run("/home/chili/ar_benchmark/data/large/",root.test_type,parseFloat(param.text))
                }
            }
        }


        Rectangle{
            id:status
            width: 100
            height: width
            radius: width/2
            color: "yellow"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.margins: 10
        }
    }


}
