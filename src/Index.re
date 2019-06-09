module Slider = {
  [@react.component]
  let make = (~min, ~max, ~step=1.0, ~label, ~onChange) => {
    let (value, setValue) =
      React.useState(() => (min + int_of_string(max)) / 2);

    let timeoutRef = React.useRef(None);

    let changeValue = e => {
      switch (React.Ref.current(timeoutRef)) {
      | None =>
        let value = ReactEvent.Synthetic.target(e)##value;
        let timeoutId =
          Js.Global.setTimeout(
            () => {
              React.Ref.setCurrent(timeoutRef, None);
              setValue(value);
            },
            10,
          );
        React.Ref.setCurrent(timeoutRef, Some(timeoutId));
      | Some(_) => ()
      };
    };

    React.useEffect1(
      () => {
        onChange(value);
        None;
      },
      [|value|],
    );

    <div>
      <input
        type_="range"
        min
        max
        step
        value={string_of_int(value)}
        onChange=changeValue
      />
      <label>
        {ReasonReact.string(label)}
        {ReasonReact.string(string_of_int(value))}
      </label>
    </div>;
  };
};

module Document = {
  let window : Dom.element = [%bs.raw "window"];

  [@bs.send]
  external addEventListener : Dom.element => string => (Dom.event => unit) => unit = "addEventListener";

  [@bs.get]
  external getWidth : Dom.element => int = "innerWidth";
  [@bs.get]
  external getHeight : Dom.element => int = "innerHeight";

  [@bs.set]
  external setWidth : Dom.element => int => unit = "width";
  [@bs.set]
  external setHeight : Dom.element => int => unit = "height";
};

module Canvas = {
  type context;

  [@bs.send]
  external getContext: (Dom.element, string) => context = "getContext";

  [@bs.send] external beginPath: context => unit = "beginPath";
  [@bs.send] external moveTo: (context, float, float) => unit = "moveTo";
  [@bs.send] external lineTo: (context, float, float) => unit = "lineTo";
  [@bs.send] external closePath: context => unit = "closePath";

  [@bs.send] external fill: context => unit = "fill";
  [@bs.send] external stroke: context => unit = "stroke";

  [@bs.send]
  external fillRect: (context, float, float, float, float) => unit =
    "fillRect";
  [@bs.send]
  external clearRect: (context, float, float, float, float) => unit =
    "clearRect";
  [@bs.send]
  external arc: (context, float, float, float, float, float, bool) => unit =
    "arc";
};

let getNthPoint = (width, height, radius, modulo, n) => {
  let n_float = float_of_int(n);
  let modulo_float = float_of_int(modulo)
  let centerX = width /. 2.;
  let centerY = height /. 2.;

  let pi = Js.Math._PI;
  let dx = radius *. Js.Math.cos(2. *. pi *. n_float /. modulo_float);
  let dy = radius *. Js.Math.sin(2. *. pi *. n_float /. modulo_float);

  (centerX +. dx, centerY +. dy);
};

let drawOnCanvas = (ctx, modulo, multiplier, width, height) => {
  let width_float = float_of_int(width);
  let height_float = float_of_int(height);
  let radius = 0.5 *. Js.Math.min_float(width_float *. 0.75, height_float *. 0.75);
  
  Canvas.clearRect(ctx, 0., 0., width_float, height_float);
  Canvas.beginPath(ctx);

  for (ii in 0 to modulo - 1) {
    let (fromX, fromY) =
      getNthPoint(width_float, height_float, radius, modulo, ii);

    let to_index = ii * multiplier mod modulo;
    let (toX, toY) =
      getNthPoint(width_float, height_float, radius, modulo, to_index);

    Canvas.moveTo(ctx, fromX, fromY);
    Canvas.lineTo(ctx, toX, toY);
  };

  Canvas.stroke(ctx);
  Canvas.closePath(ctx);
};

module ModuloTimesTable = {
  [@react.component]
  let make = (~modulo, ~multiplier, ~width, ~height) => {
    let canvasRef = React.useRef(Js.Nullable.null);

    React.useEffect4(
      () => {
        let canvas = React.Ref.current(canvasRef) |> Js.Nullable.toOption;
        switch (canvas) {
        | None => ()
        | Some(canvas) =>
          Document.setWidth(canvas, width);
          Document.setHeight(canvas, height);
          let ctx = Canvas.getContext(canvas, "2d");

          drawOnCanvas(ctx, modulo, multiplier, width, height);
        };

        None;
      },
      (modulo, multiplier, width, height),
    );

    <canvas ref={ReactDOMRe.Ref.domRef(canvasRef)}>
      {ReasonReact.string("Canvas not supported!?")}
    </canvas>;
  };
};

module App = {
  let logEvent = _event => Js.log2("app: clicked!", _event);

  let height = Document.getHeight(Document.window);
  let width = Document.getWidth(Document.window);

  [@react.component]
  let make = () => {
    let (modulo, setModulo) = React.useState(() => 10);
    let (multiplier, setMultiplier) = React.useState(() => 2);

    let changeModulo = newModulo => {
      setModulo(_ => newModulo);
    };

    let changeMultiplier = newMultiplier => {
      setMultiplier(_ => newMultiplier);
    };

    <>
      <div>
        <Slider min=3 max="1200" label="modulo" onChange=changeModulo />
        <Slider min=2 max="500" label="multiplier" onChange=changeMultiplier />
      </div>
      <ModuloTimesTable modulo multiplier height width/>
    </>;
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");