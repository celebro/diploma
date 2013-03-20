data = {}

$(document).ready(function () {
    impress().init();

    $(document).on("keydown", function(e) {
        console.log(e.which);
        switch (e.which) {
            case 27: impress().goto("overview");
        }
    });

});