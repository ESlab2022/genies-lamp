import React, { useState } from "react";
import { useMutation } from "@tanstack/react-query";
import clsx from "clsx";
import BrightnessControl from "./BrightnessControl";
import ColorControl from "./ColorControl";
import { AiFillWarning } from "react-icons/ai";
import useToasts from "../../hooks/useToasts";
export interface ControlTableRowProps {
  index: number;
  deviceId: number;
  nearby?: boolean;
  emergency?: boolean;
}

export const ControlTableRow = ({
  index,
  deviceId,
  nearby,
  emergency,
}: ControlTableRowProps) => {
  const { pushToast } = useToasts();
  const [color, setColor] = useState("#FFFFFF");
  const [brightness, setBrightness] = useState(0);
  const mutation = useMutation<
    unknown,
    unknown,
    { color: string; brightness: number }
  >({
    mutationFn: ({ color, brightness }) => {
      const colorCode = color.replace("#", "");
      const realBrightness = brightness * 100;
      return fetch(
        `http://192.168.10.36:4000/setColor?deviceID=${deviceId}&color=${colorCode}&brightness=${realBrightness}`
      );
    },
    onSuccess: () => {
      pushToast({
        type: "success",
        message: "Successfully updated light",
      });
    },
    retryDelay: 500,
  });

  const onColorChange = (color: string) => {
    mutation.mutate({ color, brightness });
    setColor(color);
  };

  const onBrightnessChange = (brightness: number) => {
    mutation.mutate({ color, brightness });
    setBrightness(brightness);
  };

  return (
    <>
      <tr
        className={clsx("border-b", "border-gray-700", {
          "bg-red-600": emergency,
          "bg-gray-800": !emergency && index % 2 === 0,
          "bg-gray-700": !emergency && index % 2 === 1,
        })}
      >
        <td className="py-4 px-6">
          {emergency && <AiFillWarning className="text-2xl" />}
        </td>
        <th
          scope="row"
          className="py-4 px-6 font-medium whitespace-nowrap text-white"
        >
          {deviceId}
        </th>
        <td className="py-4 px-6">
          <ColorControl color={color} onColorChange={onColorChange} />
        </td>
        <td className="py-4 px-6">
          <BrightnessControl
            brightness={brightness}
            onBrightnessChange={onBrightnessChange}
          />
        </td>
        <td className="py-4 px-6">{nearby}</td>
      </tr>
    </>
  );
};

export default ControlTableRow;
