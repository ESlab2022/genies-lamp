import React from "react";
import clsx from "clsx";
import BrightnessControl from "./BrightnessControl";
import ColorControl from "./ColorControl";

export interface ControlTableRowProps {
  index: number;
  deviceId: string;
  color: string;
  brightness: number;
  nearby: boolean;
  emergency: boolean;
}

export const ControlTableRow = ({
  index,
  deviceId,
  color,
  brightness,
  nearby,
  emergency,
}: ControlTableRowProps) => {
  return (
    <tr
      className={clsx("border-b", "border-gray-700", {
        "bg-red-600": emergency,
        "bg-gray-800": !emergency && index % 2 === 0,
        "bg-gray-700": !emergency && index % 2 === 1,
      })}
    >
      <th
        scope="row"
        className="py-4 px-6 font-medium whitespace-nowrap text-white"
      >
        {deviceId}
      </th>
      <td className="py-4 px-6">
        <ColorControl />
      </td>
      <td className="py-4 px-6">
        <BrightnessControl />
      </td>
      <td className="py-4 px-6">{nearby}</td>
      <td className="py-4 px-6">
        <button
          type="button"
          className={clsx(
            "focus:outline-none text-white focus:ring-4 font-medium rounded-lg text-sm px-5 py-2.5 mr-2 mb-2",
            {
              "bg-gray-600": !emergency,
              "hover:ring-gray-900": !emergency,
              "bg-blue-600": emergency,
              "hover:bg-blue-700": emergency,
              "focus:ring-blue-900": emergency,
            }
          )}
          disabled={!emergency}
        >
          resolve
        </button>
      </td>
    </tr>
  );
};

export default ControlTableRow;
