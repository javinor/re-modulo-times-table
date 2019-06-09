let debounce = (f, ms) => {
  let timerId = ref(None);

  ev => {
    switch (timerId^) {
    | Some(_) => ()
    | None =>
      let value = ReactEvent.Synthetic.target(ev)##value;
      timerId := Some(Js.Global.setTimeout(() => f(value), ms));
    };
  };
};

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

module App = {
  let logEvent = _event => Js.log2("clicked!", _event);

  [@react.component]
  let make = (~message) =>
    <>
      <div>
        <Slider min=3 max="1200" label="modulo" onChange=logEvent />
        <Slider
          min=2
          max="1003"
          step=0.002
          label="multiplier"
          onChange=logEvent
        />
      </div>
      <canvas id="canvas">
        {ReasonReact.string("Canvas not supported!?")}
      </canvas>
    </>;
};

ReactDOMRe.renderToElementWithId(
  <App message="Hello! Click this text." />,
  "app",
);