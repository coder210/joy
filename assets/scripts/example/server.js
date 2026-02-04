require("scripts/example/math.js")


var a = 10;
var t = {}
t.name = "liwei";
t.age = 10;

console.log(JSON.stringify(t));
console.log("print hello world from js");
console.log("hello world2222");

var tttt = vec2f.negate({x:10, y:20});
console.log(tttt.x + ":" + tttt.y);

MyLib1.publicMethod();


var t1 = fp.from_float(4);
var t2 = fp.sqrt(t1);
console.log("t2:" + fp.to_float(t2));

if (a == 20){
        console.log("true");
}
else{
        console.log("false");
}


var app = {}
app.start = function (ctx) {
        app.ctx = ctx;
        core.log(app.ctx, "test js scriptx:" + app.ctx);
        app.server = lockstep.create();
        //core.quit(ctx);
};

app.event = function (ev) {
        core.log(app.ctx, "js event type");
};

app.update = function () {
        lockstep.update(app.server, 0.667);
};

app.destroy = function () {
        lockstep.destroy(app.server);
        core.log(app.ctx, "test js destroy");
}


