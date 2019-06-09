// Generated by BUCKLESCRIPT VERSION 5.0.4, PLEASE EDIT WITH CARE
'use strict';

var React = require("react");
var ReactDOMRe = require("reason-react/src/ReactDOMRe.js");

function logEvent(e) {
  console.log("value changed!", e);
  return /* () */0;
}

function Index$Slider(Props) {
  var min = Props.min;
  var max = Props.max;
  var label = Props.label;
  return React.createElement("div", undefined, React.createElement("input", {
                  max: max,
                  min: min,
                  type: "range",
                  onChange: logEvent
                }), React.createElement("label", undefined, label, React.createElement("span", undefined)));
}

var Slider = /* module */[
  /* logEvent */logEvent,
  /* make */Index$Slider
];

function handleClick(_event) {
  console.log("clicked!");
  return /* () */0;
}

function Index$App(Props) {
  Props.message;
  return React.createElement(React.Fragment, undefined, React.createElement("div", undefined, React.createElement(Index$Slider, {
                      min: 3,
                      max: "1200",
                      label: "modulo"
                    }), React.createElement("div", undefined, React.createElement("input", {
                          id: "moduloRange",
                          max: "1200",
                          min: 3,
                          type: "range"
                        }), React.createElement("label", undefined, "modulo", React.createElement("span", {
                              id: "moduloRangeLabel"
                            }))), React.createElement("div", undefined, React.createElement("input", {
                          id: "multiplyRange",
                          max: "1003",
                          min: 2,
                          step: 0.002,
                          type: "range"
                        }), React.createElement("label", undefined, "multiplier", React.createElement("span", {
                              id: "multiplyRangeLabel"
                            })))), React.createElement("canvas", {
                  id: "canvas"
                }, "Canvas not supported!?"));
}

var App = /* module */[
  /* handleClick */handleClick,
  /* make */Index$App
];

ReactDOMRe.renderToElementWithId(React.createElement(Index$App, {
          message: "Hello! Click this text."
        }), "app");

exports.Slider = Slider;
exports.App = App;
/*  Not a pure module */
