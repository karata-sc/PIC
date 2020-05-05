// ==UserScript==
// @name 全角英数字を半角に
// @namespace http://localhost/
// @include *
// @description 全角英数字を半角にして表示する。
// ==/UserScript==

javascript:(function() {
  var keylst_num = new Array("０", "１", "２", "３", "４", "５", "６", "７", "８", "９");
  var replst_num = new Array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");
  var keylst_alp = new Array("Ａ", "Ｂ", "Ｃ", "Ｄ", "Ｅ", "Ｆ", "Ｇ", "Ｈ", "Ｉ", "Ｊ", "Ｋ", "Ｌ", "Ｍ", "Ｎ", "Ｏ", "Ｐ", "Ｑ", "Ｒ", "Ｓ", "Ｔ", "Ｕ", "Ｖ", "Ｗ", "Ｘ", "Ｙ", "Ｚ", "ａ", "ｂ", "ｃ", "ｄ", "ｅ", "ｆ", "ｇ", "ｈ", "ｉ", "ｊ", "ｋ", "ｌ", "ｍ", "ｎ", "ｏ", "ｐ", "ｑ", "ｒ", "ｓ", "ｔ", "ｕ", "ｖ", "ｗ", "ｘ", "ｙ", "ｚ");
  var replst_alp = new Array("A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z");
  var keylst_symb = new Array("＃", "＄", "％", "＆", "’", "＊", "＋", "，", "－", "．", "／", "＜", "＝", "＞", "＠", "［", "￥", "］", "＾", "＿", "｀", "｛", "｜", "｝");
  var replst_symb = new Array("#", "$", "%", "&", "'", "*", "+", ",", "-", ".", "/", "<", "=", ">", "@", "[", "\\", "]", "^", "_", "`", "{", "|", "}");
  //var keylst_punc = new Array("　", "！", "”", "（", "）", "：", "；", "？", "～");
  //var replst_punc = new Array(" ", "!", "\"", "(", ")", ":", ";", "?", "~");
  var keylst = keylst_num.concat(keylst_alp, keylst_symb);
  var replst = replst_num.concat(replst_alp, replst_symb);
  var re = new RegExp('(' + keylst.join("|") + ')', 'g');

  function convertchar(c) {
    for (var i = 0; i < keylst.length; i++) {
      if (keylst[i] == c) {
        return replst[i];
      }
    }
    return c;
  }

  var nodes = document.evaluate('//descendant::text()', document, null,
                                XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
  var len = nodes.snapshotLength;
  var node, tmp;
  for (var i = 0; i < len; i++) {
    node = nodes.snapshotItem(i);
    node.nodeValue = node.nodeValue.replace(re, convertchar);
  }
})();
