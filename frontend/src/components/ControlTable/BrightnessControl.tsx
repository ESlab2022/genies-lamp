import React from "react";

export const BrightnessControl = () => {
  return (
    <div className="w-full flex flex-row gap-8 items-center justify-center">
      <input
        type="range"
        min="0"
        max="1"
        step="0.1"
        className="w-32 h-2 rounded-lg appearance-none cursor-pointer bg-gray-500"
        // value="2.5"
      />
      <span>10</span>
    </div>
  );
};

export default BrightnessControl;
