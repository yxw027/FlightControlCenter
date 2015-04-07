import QtQuick 2.2
import Material 0.1
import Material.ListItems 0.1 as ListItem

ApplicationWindow {
    id: fgwindow

    property QtObject fgController: null

    theme {
        accentColor: "#009688"
    }

    initialPage: page
    Page {
        id: page
        title: "Flight Gear Autopilot"

        actions: [
            Action {
                iconName: "image/color_lens"
                name: "Colors"
                onTriggered: themeChanger.show()
            },

            Action {
                iconName: "action/search"
                name: "Search"
            },

            //            Action {
            //                iconName: "action/language"
            //                name: "Language"
            //            },

            //            Action {
            //                iconName: "action/account_circle"
            //                name: "Account"
            //            },

            Action {
                iconName: "action/settings"
                name: "Settings"
            }
        ]

        Sidebar {
            id: sidebar

            FgAircraftsView {
                id: aircraftsView
           }

        }
        Flickable {
            id: flickable
            anchors {
                left: sidebar.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            clip: true
            contentHeight: Math.max(example.implicitHeight + 40, height)

            Loader {
                id: example
                anchors.fill: parent
                asynchronous: true
                source: Qt.resolvedUrl(aircraftsView.currentAiPagePath)
            }
        }
        Scrollbar {
            flickableItem: flickable
        }
    }

    FgThemeChanger { id: themeChanger}
}