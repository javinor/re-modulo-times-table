module DebouncedSlider = {
  [@react.component]
  let make = (~min, ~max, ~step, ~label, ~value, ~onChange) => {
    let timeoutRef = React.useRef(None);

    let changeValue = e => {
      switch (React.Ref.current(timeoutRef)) {
      | None =>
        let value = Js.String.make(ReactEvent.Synthetic.target(e)##value);
        let timeoutId =
          Js.Global.setTimeout(
            () => {
              React.Ref.setCurrent(timeoutRef, None);
              onChange(value);
            },
            10,
          );
        React.Ref.setCurrent(timeoutRef, Some(timeoutId));
      | Some(_) => ()
      };
    };

    <div>
      <input type_="range" min max step value onChange=changeValue />
      <label> {ReasonReact.string(label ++ " " ++ value)} </label>
    </div>;
  };
};

module Document = {
  let window: Dom.element = [%bs.raw "window"];

  [@bs.send]
  external addEventListener: (Dom.element, string, Dom.event => unit) => unit =
    "addEventListener";

  [@bs.send]
  external removeEventListener:
    (Dom.element, string, Dom.event => unit) => unit =
    "removeEventListener";

  type animationFrameID;
  [@bs.val]
  external requestAnimationFrame: (unit => unit) => animationFrameID = "";
  [@bs.val] external cancelAnimationFrame: animationFrameID => unit = "";

  [@bs.get] external getWidth: Dom.element => int = "innerWidth";
  [@bs.get] external getHeight: Dom.element => int = "innerHeight";

  [@bs.set] external setWidth: (Dom.element, int) => unit = "width";
  [@bs.set] external setHeight: (Dom.element, int) => unit = "height";
};

module Canvas = {
  type context;

  [@bs.send]
  external getContext: (Dom.element, string) => context = "getContext";

  [@bs.send]
  external clearRect: (context, float, float, float, float) => unit =
    "clearRect";

  [@bs.send] external beginPath: context => unit = "beginPath";
  [@bs.send] external moveTo: (context, float, float) => unit = "moveTo";
  [@bs.send] external lineTo: (context, float, float) => unit = "lineTo";
  [@bs.send] external stroke: context => unit = "stroke";
  [@bs.send] external closePath: context => unit = "closePath";
};

let getNthPoint = (width, height, radius, modulo, n) => {
  let n_float = float_of_int(n);
  let modulo_float = float_of_int(modulo);
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
  let radius =
    0.5 *. Js.Math.min_float(width_float *. 0.75, height_float *. 0.75);

  Canvas.clearRect(ctx, 0., 0., width_float, height_float);
  Canvas.beginPath(ctx);

  for (ii in 0 to modulo - 1) {
    let (fromX, fromY) =
      getNthPoint(width_float, height_float, radius, modulo, ii);

    let to_index = int_of_float(float_of_int(ii) *. multiplier) mod modulo;
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
  let getWindowSize = () => Document.(getWidth(window), getHeight(window));

  let multiplierMin = 2.;
  let multiplierMax = 20.;
  let multiplierStep = 0.01;

  [@react.component]
  let make = () => {
    let (frameId, setFrameId) = React.useState(() => None);
    let ((width, height), setSize) = React.useState(getWindowSize);
    let (modulo, setModulo) = React.useState(() => 571);
    let (multiplier, setMultiplier) = React.useState(() => 2.);

    React.useEffect(() => {
      let updateSize = _ => setSize(_ => getWindowSize());
      Document.(addEventListener(window, "resize", updateSize));
      Some(
        () => Document.(removeEventListener(window, "resize", updateSize)),
      );
    });

    React.useEffect(() => {
      switch (frameId) {
      | None =>
        let animationFrameId =
          Document.requestAnimationFrame(() => {
            setMultiplier(mult =>
              mult >= multiplierMax ? multiplierMin : mult +. multiplierStep
            );
            setFrameId(_ => None);
          });
        setFrameId(_ => Some(animationFrameId));
      | Some(_) => ()
      };

      None;
    });

    <>
      <div className="controls">
        <DebouncedSlider
          min=3
          max="600"
          step=1.
          value={string_of_int(modulo)}
          label="modulo"
          onChange={newVal => setModulo(_ => int_of_string(newVal))}
        />
        <DebouncedSlider
          min={int_of_float(multiplierMin)}
          max={Js.Float.toString(multiplierMax)}
          step=multiplierStep
          label="multiplier"
          value={Js.Float.toFixedWithPrecision(~digits=3, multiplier)}
          onChange={newVal => setMultiplier(_ => float_of_string(newVal))}
        />
      </div>
      <ModuloTimesTable modulo multiplier height width />
    </>;
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app");