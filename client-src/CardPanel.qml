import QtQuick

Item {
    id: root
    property var cards: []
    property var initial_x: []
    property bool rtl: false
    Component {
        id: card
        Card {
            Behavior on x {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuart
                }
            }
            onHover: function () {
                for (var i = card_id + 1; i < cards.length; ++i) {
                    cards[i].x += 66 * (-1) ** rtl
                }
            }
            onLeave: function () {
                for (var i = card_id + 1; i < cards.length; ++i) {
                    cards[i].x = root.initial_x[i]
                }
            }
        }
    }
    function add_card(_suit, _rank, _id) {
        var xx = 0 + root.rtl * (root.width - 200)
        if (_id !== 0) {
            xx = cards[_id - 1].x + 66 * (-1) ** root.rtl
        }
        initial_x[_id] = xx
        var m = card.createObject(root, {
                                      "card_id": _id,
                                      "suit": _suit,
                                      "rank": _rank,
                                      "height": 271,
                                      "width": 200,
                                      "x": xx
                                  })
        cards.push(m)
    }

    Component.onCompleted: {
        for (var i = 0; i < 4; ++i) {
            add_card(2, i, i)
        }
    }
}
