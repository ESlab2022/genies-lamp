import React, { useCallback, useState } from "react";
import debounce from "lodash/debounce";

export interface BrightnessControlProps {
  onBrightnessChange: (brightness: number) => void;
}

export const BrightnessControl = ({
  onBrightnessChange,
}: BrightnessControlProps) => {
  const [brightness, setBrightness] = useState(0);

  const debouncedBrightnessChange = useCallback(
    debounce(onBrightnessChange, 500),
    [onBrightnessChange]
  );

  const handleBrightnessChange = (brightness: number) => {
    setBrightness(brightness);
    debouncedBrightnessChange(brightness);
  };

  return (
    <div className="flex flex-row gap-8 items-center justify-center">
      <input
        type="range"
        min="0"
        max="1"
        step="0.1"
        className="w-32 h-2 rounded-lg appearance-none cursor-pointer bg-gray-500"
        value={brightness}
        onChange={(e) => handleBrightnessChange(Number(e.target.value))}
      />
      <span className="w-4 text-right">{brightness}</span>
    </div>
  );
};

export default BrightnessControl;
