$(function(){
    var requestMethod = function(type) {
        return function(e) {
            var target = $(e.target).attr('data-value');
            $.ajax({
                url: '/update',
                method: 'POST',
                data: type + '=' + target
            });
        };
    };

    $('.line').click(requestMethod('line'));
    $('.length').click(requestMethod('length'));
    $('.position').click(requestMethod('position'));
    $('.destinations').click(requestMethod('destination'));
});
