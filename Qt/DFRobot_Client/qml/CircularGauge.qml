import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtQuick.Extras.Private 1.0

Rectangle
{
    id: root
    width: circular_gauge.width + 5
    height: circular_gauge.height + 5
    color: "#494d53"

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    CircularGauge
    {
        id: circular_gauge
        objectName: "circular_gauge"

        width: 200
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        //Setting custom value;
        //value: 80

        //Testing purpose
        /*
        value: accelerating ? maximumValue : 0
        anchors.centerIn: parent

        property bool accelerating: false

        Keys.onSpacePressed: accelerating = true
        Keys.onReleased:
        {
            if (event.key === Qt.Key_Space)
            {
                accelerating = false;
                event.accepted = true;
            }
        }
        Component.onCompleted: forceActiveFocus()
       */

        Behavior on value
        {
            NumberAnimation
            {
                duration: 300 //how fast you want to rotate
            }
        }

        style: CircularGaugeStyle
        {

            function degreesToRadians(degrees)
            {
                return degrees * (Math.PI / 180);
            }

            background: Canvas
            {
                onPaint:
                {
                    var ctx = getContext("2d");
                    ctx.reset();

                    ctx.beginPath();
                    ctx.strokeStyle = "#e34c22";
                    ctx.lineWidth = outerRadius * 0.02;

                    ctx.arc(outerRadius, outerRadius, outerRadius - ctx.lineWidth / 2,
                        degreesToRadians(valueToAngle(80) - 90), degreesToRadians(valueToAngle(100) - 90));
                    ctx.stroke();
                }
            }

            tickmark: Rectangle
            {
                visible: styleData.value < 80 || styleData.value % 10 == 0
                implicitWidth: outerRadius * 0.02
                antialiasing: true
                implicitHeight: outerRadius * 0.06
                color: styleData.value >= 80 ? "#e34c22" : "#e5e5e5"
            }

            minorTickmark: Rectangle
            {
                visible: styleData.value < 80
                implicitWidth: outerRadius * 0.01
                antialiasing: true
                implicitHeight: outerRadius * 0.03
                color: "#e5e5e5"
            }

            tickmarkLabel:  Text
            {
                font.pixelSize: Math.max(6, outerRadius * 0.1)
                text: styleData.value
                color: styleData.value >= 80 ? "#e34c22" : "#e5e5e5"
                antialiasing: true
            }

            needle: Rectangle
            {
                y: outerRadius * 0.15
                implicitWidth: outerRadius * 0.03
                implicitHeight: outerRadius * 0.9
                antialiasing: true
                color: "#e5e5e5"
            }

            foreground: Item
            {
                Rectangle
                {
                    width: outerRadius * 0.2
                    height: width
                    radius: width / 2
                    color: "#e5e5e5"
                    anchors.centerIn: parent
                }
            }

        }
    }
}
