$(function() {
  $(".panel").each(function(a) {
    if ($(a).hasClass("collapsed")) {
      $(".panel-title", a).append($('<span class="glyphicon glyphicon-chevron-down pull-right"></span>'));
    } else {
      $(".panel-title", a).append($('<span class="glyphicon glyphicon-chevron-up pull-right"></span>'));
    }
    $(".panel-heading", a).click(function() {
      $("span", this).toggleClass("glyphicon-chevron-up").toggleClass("glyphicon-chevron-down");
      $(this).parent().toggleClass("collapsed");
    });
  });
});
