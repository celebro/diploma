data = {}
data.nodeOriginX = 400;
data.nodeOriginY = 400;

function addNode(svg, node) {
    svg.circle(data.nodeOriginX, data.nodeOriginY, 10,
        {fill: 'red', stroke:'black', strokeWidth: 1, class_: "node-" + node}); 
}

function addEdge(svg, node1, node2) {
    svg.line(data.nodeOriginX, data.nodeOriginY, data.nodeOriginX, data.nodeOriginY,
        {stroke: "black", strokeWidth: 1, class_: "edge-" + node1 + "-" + node2 + " node1-" + node1 + " node2-" + node2});
}

function moveNode(location, node, x, y) {
    var nodes = $(location).find(".node-" + node);
    nodes.animate({svgCx: data.nodeOriginX + x, svgCy: data.nodeOriginY + y},  { duration: 2000, queue: false });
    
    var lines1 = $(location).find(".node1-" + node);
    lines1.animate({svgX1: data.nodeOriginX + x, svgY1: data.nodeOriginY + y}, { duration: 2000, queue: false });

    var lines2 = $(location).find(".node2-" + node);
    lines2.animate({svgX2: data.nodeOriginX + x, svgY2: data.nodeOriginY + y}, { duration: 2000, queue: false });
}

$(document).ready(function () {
    impress().init();

    var stepNext = function() {
        console.log("called next");
        $(".step.present").each(function() {
            var step;
            if ($(this).data("step")) {
                step = $(this).data("step");
            } else {
                step = 0;
            }
            var calls = $(this).data("calls");
            if (calls && calls.length > step + 1) {
                step = step + 1;
                console.log("entered step " + step);
                calls[step]();
            } else {
                console.log("stayed on step " + step);
            }
            $(this).data("step", step);
        });
    };

    var stepBack = function() {
        console.log("called back");
        $(".step.present").each(function() {
            var step;
            if ($(this).data("step")) {
                step = $(this).data("step");
            } else {
                step = 0;
            }
            var calls = $(this).data("calls");
            if (step - 1 >= 0) {
                step = step - 1;
                console.log("entered step " + step);
                calls[step]();
            } else {
                console.log("stayed on step " + step);
            }
            $(this).data("step", step);
        });
    };

    $(document).on("keydown", function(e) {
        console.log(e.which);
        switch (e.which) {
            case 27: 
                impress().goto("overview");
                break;
            case 13: 
            case 102:
                stepNext();
                break;
            case 66:
            case 100:
                stepBack();
                break;
        }
    });

    $("#testgraph").each(function() {
        var calls = [];
        var loc = $(this).find(".graphContainer");
        var svg = loc.svg(function(svg) {
            addEdge(svg, "a", "b");
            addNode(svg, "a");
            addNode(svg, "b");
        });
        

        calls[0] = function() {
            console.log("processing step 0");

            moveNode(loc, "a", -100, -50);
            moveNode(loc, "b", 100, -40);
        };

        calls[1] = function() {
            console.log("processing step 1");
            
            moveNode(loc, "a", 100, 50);
            moveNode(loc, "b", 0, 0);
        }

        $(this).data("calls", calls);
        calls[0]();
    });

    $("circle")
        .draggable()
        .bind("mousedown", function(event, ui) {
            // bring target to front
            //$(event.target.parentElement).append(event.target);
        })
        .bind("drag", function(event, ui) {
            event.target.setAttribute("cx", ui.position.left);
            event.target.setAttribute("cy", ui.position.top);
        });

});