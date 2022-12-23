import React, { useState } from "react";
import clsx from "clsx";
import { HexColorPicker } from "react-colorful";

export interface ColorControlProps {
  color: string;
  onColorChange: (color: string) => void;
}

export const ColorControl = ({ color, onColorChange }: ColorControlProps) => {
  const [modalOpen, setModalOpen] = useState(false);
  const [internalColor, setInternalColor] = useState(color);

  const openModal = () => {
    setModalOpen(true);
  };

  const closeModal = () => {
    setModalOpen(false);
  };

  const handleColorChange = (color: string) => {
    setInternalColor(color);
  };

  const handleSubmit = () => {
    onColorChange(internalColor);
    closeModal();
  };

  return (
    <>
      <button
        className="block focus:ring-4 focus:outline-none font-medium rounded-lg text-sm px-5 py-2.5 text-center w-24"
        type="button"
        data-modal-toggle="defaultModal"
        style={{
          backgroundColor: internalColor,
        }}
        onClick={openModal}
      >
        {internalColor}
      </button>

      <div
        className={clsx(
          "fixed top-0 left-0 right-0 z-50 h-full w-full grid place-content-center bg-black bg-opacity-50 backdrop-blur-sm",
          modalOpen ? "flex" : "hidden"
        )}
      >
        <div
          id="defaultModal"
          tab-index="-1"
          aria-hidden="true"
          className="p-4 overflow-x-hidden overflow-y-auto md:inset-0 h-modal md:h-full"
        >
          <div className="relative w-full h-full max-w-2xl md:h-auto">
            <div className="relative rounded-lg shadow bg-gray-700">
              <div className="flex items-start justify-between p-4 border-b rounded-t border-gray-600">
                <h3 className="text-xl font-semibold text-white">
                  Pick a new color
                </h3>
              </div>
              <div className="p-6 px-12">
                <HexColorPicker color={color} onChange={handleColorChange} />
              </div>
              <div className="flex items-center justify-end p-6 space-x-2 border-t rounded-b border-gray-600">
                <button
                  data-modal-toggle="defaultModal"
                  type="button"
                  className="text-white focus:ring-4 focus:outline-none font-medium rounded-lg text-sm px-5 py-2.5 text-center bg-blue-600 hover:bg-blue-700 focus:ring-blue-800"
                  onClick={handleSubmit}
                >
                  Save
                </button>
                <button
                  data-modal-toggle="defaultModal"
                  type="button"
                  className="focus:ring-4 focus:outline-none rounded-lg border text-sm font-medium px-5 py-2.5 focus:z-10 bg-gray-700 text-gray-300 border-gray-500 hover:text-white hover:bg-gray-600 focus:ring-gray-600"
                  onClick={closeModal}
                >
                  Cancel
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </>
  );
};

export default ColorControl;
