import React from "react";

export interface BrightnessControlProps {
  brightness: number;
  onBrightnessChange: (brightness: number) => void;
}

export const BrightnessControl = ({
  brightness,
  onBrightnessChange,
}: BrightnessControlProps) => {
  const handleBrightnessChange = (brightness: number) => {
    onBrightnessChange(brightness);
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
