data = {}
data.nodeOriginX = 750;
data.nodeOriginY = 500;
data.duration = 1000;
data.node = {};
data.label = {};
data.label.x = -3;
data.label.y = 4;

String.prototype.format = function() {
    var formatted = this;
    for(arg in arguments) {
        formatted = formatted.replace("{" + arg + "}", arguments[arg]);
    }
    return formatted;
};

function addNode(svg, node, showLabel) {
    svg.circle(data.nodeOriginX, data.nodeOriginY, 10,
        {fill: 'red', stroke:'black', strokeWidth: 1, class_: "node-" + node, "data-label": node}); 
    if(showLabel) {
        svg.text(data.nodeOriginX, data.nodeOriginY, node, {class_: "nodeLabel label-" + node});
    }
}

function addEdge(svg, node1, node2) {
    svg.line(data.nodeOriginX, data.nodeOriginY, data.nodeOriginX, data.nodeOriginY,
        {stroke: "black", strokeWidth: 1, class_: "edge-" + node1 + "-" + node2 + " node1-" + node1 + " node2-" + node2});
}

function moveNode(location, node, x, y) {
    var nodes = $(location).find(".node-" + node);
    nodes.animate({svgCx: data.nodeOriginX + x, svgCy: data.nodeOriginY + y},  { duration: data.duration, queue: false });
    
    var lines1 = $(location).find(".node1-" + node);
    lines1.animate({svgX1: data.nodeOriginX + x, svgY1: data.nodeOriginY + y}, { duration: data.duration, queue: false });

    var lines2 = $(location).find(".node2-" + node);
    lines2.animate({svgX2: data.nodeOriginX + x, svgY2: data.nodeOriginY + y}, { duration: data.duration, queue: false });

    var labels = $(location).find(".label-" + node);
    labels.animate({svgX: data.nodeOriginX + x, svgY: data.nodeOriginY + y}, { duration: data.duration, queue: false });
}

function setNodePos(location, node, x, y) {
    var nodes = $(location).find(".node-" + node);
    nodes.attr("cx", x);
    nodes.attr("cy", y);
    
    var lines1 = $(location).find(".node1-" + node);
    lines1.attr("x1", x);
    lines1.attr("y1", y);

    var lines2 = $(location).find(".node2-" + node);
    lines2.attr("x2", x);
    lines2.attr("y2", y);

    var labels = $(location).find(".label-" + node);
    labels.attr("x", x);
    labels.attr("y", y);    
}

function printGraphPos() {
    var nodes = $(".present circle");
    var output = ""
    for(var i = 0; i < nodes.length; i++) {
        var node = $(nodes[i])
        //console.log("node" + node.attr("cx") + " " + node.attr("cy"));
        //moveNode(loc, "a", -100, -50);
        output += 'moveNode(loc, "{0}", {1}, {2});\n'.format(node.attr("data-label"), parseFloat(node.attr("cx")) - data.nodeOriginX, parseFloat(node.attr("cy")) - data.nodeOriginY);
    }
    console.log(output);
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
            addNode(svg, "a", true);
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
        .draggable({
            start: function(event, ui) {            
                data.node.x = parseFloat($(this).attr("cx"));
                data.node.y = parseFloat($(this).attr("cy"));
            },
            drag: function(event, ui) {
                
                var dx = ui.position.left - ui.originalPosition.left
                var dy = ui.position.top - ui.originalPosition.top;
                var x = dx + data.node.x;
                var y = dy + data.node.y;
                setNodePos($(this).parents(".graphContainer"), $(this).attr("data-label"), x, y);
                //event.target.setAttribute("cx", ui.position.left);
                //event.target.setAttribute("cy", ui.position.top);
                //console.log(data.node.x + "/" + data.node.y + "   " + dx + "/" + dy + "   " + x + "/" + y);
            }
        });

});