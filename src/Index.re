module Slider = {
  let logEvent = e => {
    // let value = e.target.value; 
    Js.log2("value changed!", e)
  };

  [@react.component]
  let make = (~min, ~max, ~label) => {
    <div>
      <input type_="range" min max onChange=logEvent />
      <label> {ReasonReact.string(label)} <span /> </label>
    </div>;
  };
};

module App = {
  let handleClick = _event => Js.log("clicked!");

  [@react.component]
  let make = (~message) =>
    <>
      <div>
        <Slider min=3 max="1200" label="modulo" />
        <div>
          <input id="moduloRange" type_="range" min=3 max="1200" />
          <label>
            {ReasonReact.string("modulo")}
            <span id="moduloRangeLabel" />
          </label>
        </div>
        <div>
          <input
            id="multiplyRange"
            type_="range"
            min=2
            max="1003"
            step=0.002
          />
          <label>
            {ReasonReact.string("multiplier")}
            <span id="multiplyRangeLabel" />
          </label>
        </div>
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