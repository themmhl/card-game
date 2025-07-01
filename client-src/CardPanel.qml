import QtQuick

Item {
    id: root
    property var cards: []
    property var initial_x: []
    property int _id: 0
    property bool rtl: false
    property bool select_enabled: false
    function clearCards() {
        for (var i = 0; i < cards.length; ++i) {
            cards[i].destroy();
        }
        cards = [];
    }
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
            onClicked:function(){
                if(select_enabled){
                    server.select_card(suit, rank)
                }
            }
        }
    }
    function add_card(_suit, _rank) {
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
        _id++;
    }
}
