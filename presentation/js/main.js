data = {}
data.nodeOriginX = 750;
data.nodeOriginY = 500;
data.duration = 1000;
data.originalDuration = 1000;
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

function addNode(svg, node, className) {
    if (!className) {
        className = "";
    }

    svg.circle(data.nodeOriginX, data.nodeOriginY, 10,
        {fill: 'red', stroke:'black', strokeWidth: 1, class_: className + " node-" + node, "data-label": node}); 
    svg.text(data.nodeOriginX, data.nodeOriginY, node, {class_: "nodeLabel label-" + node});
}

function addEdge(svg, node1, node2, className) {
    if (!className) {
        className = "";
    }

    svg.line(data.nodeOriginX, data.nodeOriginY, data.nodeOriginX, data.nodeOriginY,
        {stroke: "black", strokeWidth: 1, class_: className + " edge edge-" + node1 + "-" + node2 + " node1-" + node1 + " node2-" + node2});
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
        var node = $(nodes[i]);
        output += 'moveNode(loc, "{0}", {1}, {2});\n'.format(node.attr("data-label"), parseFloat(node.attr("cx")) - data.nodeOriginX, parseFloat(node.attr("cy")) - data.nodeOriginY);
    }
    console.log(output);
}

function addArrowDefs(svg) {
    var defs = svg.defs();
    var marker = svg.marker(defs, "Arrow", "10", "0.0", null, null, "auto", {style: "overflow:visible"});
    svg.path(marker, "M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z",
                {transform: "scale(1.1) rotate(180) translate(1,0)"});
}

$(document).ready(function () {
    impress().init();
    consoleNotes().init();

    var stepNext = function() {
        //console.log("called next");
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
                data.duration = data.originalDuration;
                $(this).removeClass("part" + (step-1));
                $(this).addClass("part" + step);
            } else {
                console.log("stayed on step " + step);
            }
            $(this).data("step", step);
        });
    };

    var stepBack = function() {
        //console.log("called back");
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
                data.duration = data.originalDuration;
                $(this).removeClass("part" + (step+1));
                $(this).addClass("part" + step);
            } else {
                console.log("stayed on step " + step);
            }
            $(this).data("step", step);
        });
    };

    $(document).on("keydown", function(e) {
        //console.log(e.which);
        switch (e.which) {
            case 27: // esc
                impress().goto("overview");
                break;
            case 38: // up
            case 102: // 6
                stepNext();
                e.preventDefault();
                break;
            case 40: // down
            case 100: // 4
                stepBack();
                break;
        }
    });

    $("#def").each(function() {
        var calls = [];
        var loc = $(this).find(".graphContainer");
        var svg = loc.svg(function(svg) {
            addArrowDefs(svg);

            // Target
            addEdge(svg, "1", "2", "target");
            addEdge(svg, "1", "3", "target");
            addEdge(svg, "1", "4", "target");
            addEdge(svg, "2", "3", "target");
            addEdge(svg, "2", "4", "target");
            addEdge(svg, "4", "3", "target");
            addEdge(svg, "1", "5", "target");
            addEdge(svg, "5", "7", "target");
            addEdge(svg, "6", "4", "target");
            addEdge(svg, "3", "7", "target");
            addEdge(svg, "7", "6", "target");

            addNode(svg, "1", "target");
            addNode(svg, "2", "target");
            addNode(svg, "3", "target");
            addNode(svg, "4", "target");
            addNode(svg, "5", "target");
            addNode(svg, "6", "target");
            addNode(svg, "7", "target");

            // Pattern
            addEdge(svg, "a", "b", "pattern");
            addEdge(svg, "a", "c", "pattern");
            addEdge(svg, "a", "d", "pattern");
            addEdge(svg, "b", "d", "pattern");
            addEdge(svg, "d", "c", "pattern");
            
            addNode(svg, "a", "pattern");
            addNode(svg, "b", "pattern");
            addNode(svg, "c", "pattern");
            addNode(svg, "d", "pattern");

        });
        

        calls[0] = function() {
            data.duration = 2000;
            moveNode(loc, "1", -161, -7);
            moveNode(loc, "2", 134, -53);
            moveNode(loc, "3", 6, 40);
            moveNode(loc, "4", 31, 173);
            moveNode(loc, "5", -189, 81);
            moveNode(loc, "6", -82, 204);
            moveNode(loc, "7", -150, 147);
            moveNode(loc, "a", -479, 22);
            moveNode(loc, "b", -356, 25);
            moveNode(loc, "c", -341, 157);
            moveNode(loc, "d", -500, 158);
        };

        calls[1] = function() {
            data.duration = 2000;
            moveNode(loc, "1", -161, -7);
            moveNode(loc, "2", 134, -53);
            moveNode(loc, "3", 6, 40);
            moveNode(loc, "4", 31, 173);
            moveNode(loc, "5", -189, 81);
            moveNode(loc, "6", -82, 204);
            moveNode(loc, "7", -150, 147);
            moveNode(loc, "a", -161, -7);
            moveNode(loc, "b", 134, -53);
            moveNode(loc, "c", 6, 40);
            moveNode(loc, "d", 31, 173);

            $(".step.present .text2").removeClass("show");
        }

        calls[2] = function() {
            data.duration = 2000;
            moveNode(loc, "1", -161, -7);
            moveNode(loc, "2", 134, -53);
            moveNode(loc, "3", 6, 40);
            moveNode(loc, "4", 31, 173);
            moveNode(loc, "5", -189, 81);
            moveNode(loc, "6", -82, 204);
            moveNode(loc, "7", -150, 147);
            moveNode(loc, "a", -161, -7);
            moveNode(loc, "b", 134, -53);
            moveNode(loc, "c", 6, 40);
            moveNode(loc, "d", 31, 173);

            $(".step.present .text2").addClass("show");
        };

        calls[3] = function() {
            data.duration = 2000;
            moveNode(loc, "1", -161, -7);
            moveNode(loc, "2", 134, -53);
            moveNode(loc, "3", 6, 40);
            moveNode(loc, "4", 31, 173);
            moveNode(loc, "5", -189, 81);
            moveNode(loc, "6", -82, 204);
            moveNode(loc, "7", -150, 147);
            moveNode(loc, "a", -479, 22);
            moveNode(loc, "b", -356, 25);
            moveNode(loc, "c", -341, 157);
            moveNode(loc, "d", -500, 158);
        };

        $(this).data("calls", calls);
        calls[0]();
    });


    $("#variations").each(function() {
        var calls = [];
        
        calls[0] = function() {
            
        };

        calls[1] = function() {
            
        };

        $(this).data("calls", calls);
        calls[0]();
    });

    $("#testgraph").each(function() {
        var calls = [];
        var loc = $(this).find(".graphContainer");
        var svg = loc.svg(function(svg) {
            addArrowDefs(svg);

            addEdge(svg, "a", "b");

            addNode(svg, "a", true);
            addNode(svg, "b");
        });
        

        calls[0] = function() {
            //console.log("processing step 0");

            moveNode(loc, "a", -100, -50);
            moveNode(loc, "b", 100, -40);
        };

        calls[1] = function() {
            //console.log("processing step 1");
            
            moveNode(loc, "a", 100, 50);
            moveNode(loc, "b", 0, 0);
        };



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