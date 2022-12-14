import React, { useCallback, useContext, useEffect, useMemo } from "react";
import ReactDOM from "react-dom";

export interface ToastData {
  type: "success" | "warning";
  message: string;
}

// create a context for the toast
const ToastContext = React.createContext<{
  toasts: React.ReactNode[];
  pushToast: (toastData: ToastData) => void;
}>({
  toasts: [],
  pushToast: () => {},
});

export const useToasts = () => useContext(ToastContext);

export const Toast = ({ type, message }: ToastData) => {
  if (type === "warning") {
    return (
      <div
        id="toast-warning"
        className="flex items-center p-4 w-full max-w-xs rounded-lg shadow text-gray-400 bg-gray-800"
        role="alert"
      >
        <div className="inline-flex flex-shrink-0 justify-center items-center w-8 h-8 rounded-lg bg-orange-700 text-orange-200">
          <svg
            aria-hidden="true"
            className="w-5 h-5"
            fill="currentColor"
            viewBox="0 0 20 20"
            xmlns="http://www.w3.org/2000/svg"
          >
            <path
              fillRule="evenodd"
              d="M8.257 3.099c.765-1.36 2.722-1.36 3.486 0l5.58 9.92c.75 1.334-.213 2.98-1.742 2.98H4.42c-1.53 0-2.493-1.646-1.743-2.98l5.58-9.92zM11 13a1 1 0 11-2 0 1 1 0 012 0zm-1-8a1 1 0 00-1 1v3a1 1 0 002 0V6a1 1 0 00-1-1z"
              clipRule="evenodd"
            ></path>
          </svg>
          <span className="sr-only">Warning icon</span>
        </div>
        <div className="ml-3 text-sm font-normal">{message}</div>
      </div>
    );
  } else {
    return (
      <div
        id="toast-success"
        className="flex items-center p-4 w-full max-w-xs rounded-lg shadow text-gray-400 bg-gray-800"
        role="alert"
      >
        <div className="inline-flex flex-shrink-0 justify-center items-center w-8 h-8 rounded-lg bg-green-800 text-green-200">
          <svg
            aria-hidden="true"
            className="w-5 h-5"
            fill="currentColor"
            viewBox="0 0 20 20"
            xmlns="http://www.w3.org/2000/svg"
          >
            <path
              fillRule="evenodd"
              d="M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z"
              clipRule="evenodd"
            ></path>
          </svg>
          <span className="sr-only">Check icon</span>
        </div>
        <div className="ml-3 text-sm font-normal">{message}</div>
      </div>
    );
  }
};

const createContainer = () => {
  const container = document.createElement("div");
  container.setAttribute("id", "toast-container");
  const classNames = [
    "fixed",
    "bottom-6",
    "right-6",
    "z-50",
    "flex",
    "flex-col",
    "items-end",
    "gap-4",
  ];
  container.classList.add(...classNames);
  return container;
};

export const ToastProvider = ({ children }: { children: React.ReactNode }) => {
  const [toastData, setToastData] = React.useState<ToastData[]>([]);
  const container = React.useRef<HTMLDivElement | null>(null);

  const toastPortals = useMemo(() => {
    return toastData.map((toastData) => {
      return (
        container.current &&
        ReactDOM.createPortal(
          <Toast {...toastData} />,
          container.current as HTMLDivElement
        )
      );
    });
  }, [toastData, container.current]);

  const pushToast = useCallback((toastData: ToastData) => {
    // max 5 toasts
    setToastData((prev) => [...prev, toastData].slice(-5));
  }, []);

  useEffect(() => {
    container.current = createContainer();
    document.body.appendChild(container.current);
    return () => {
      if (container.current) {
        document.body.removeChild(container.current);
      }
    };
  }, []);

  useEffect(() => {
    const interval = setInterval(() => {
      setToastData((prev) => prev.slice(1));
    }, 5000);
    return () => clearInterval(interval);
  }, []);

  return (
    <ToastContext.Provider
      value={{
        toasts: toastPortals,
        pushToast,
      }}
    >
      {children}
    </ToastContext.Provider>
  );
};

export default useToasts;
