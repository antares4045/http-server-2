import React, {
  useCallback,
  useEffect,
  useReducer,
  useRef,
  useState,
} from "react";
import { Redirect } from "react-router-dom";

import CheckBoxIcon from "@mui/icons-material/CheckBox";
import ArrowBackIcon from "@mui/icons-material/ArrowBack";

import Dialog from "@mui/material/Dialog";

import Fab from "@mui/material/Fab";

import { useDropzone } from "react-dropzone";
import DNDList from "components/DNDList";

import throttle from 'lodash/throttle'


import use$ui from "store/ui";
import use$data from "store/data";


import ImageCardConstructorFactory, {
  getItemStyle,
  getListStyle,
} from "components/ImageCardConstructorFactory";
import useSendFiles from "hooks/useSendFiles";
import { Tooltip } from "@mui/material";
import useFetch from "hooks/useFetch";
import links from "constants/links";


import "../startPageStyle.css";
function prevent(event) {
  event.stopPropagation();
}

function FileCard({ activeFile }) {
  const [{ files, parse_pages_result }, { updateFiles }] = use$data();
  const currentFile = files.find(({ id }) => id == activeFile);

  const rects = (
    parse_pages_result.find(({ id }) => id == activeFile) || { rects: [] }
  ).rects;
  const ref = useRef();
  const [size, updateSize] = useReducer(
    (acc) => {
      const bbox = ref.current && ref.current.getBoundingClientRect();
      if (!bbox) {
        setTimeout(updateSize, 100);
        return acc;
      }
      return [bbox.width, bbox.height];
    },
    ["100%", "100%"]
  );

  useEffect(() => {
    setTimeout(updateSize, 100);
  }, [currentFile]);

  return (
    <div
      style={{
        overflow: "auto",
        height: "100%",
        width: "100%",
        maxWidth: "70vw",
      }}
    >
      <div style={{ position: "relative" }}>
        <img
          src={currentFile.preview}
          ref={ref}
          alt={currentFile.file.name}
        ></img>
        <svg
          style={{ position: "absolute", top: 0, left: 0 }}
          width={size[0]}
          height={size[1]}
        >
          {rects.map(({ coords, literal }) => {
            const path =
              "M" +
              coords
                .concat([coords[0]])
                .map(([x, y]) => `${x} ${y}`)
                .join("L");
            return (
              <Tooltip title={literal}>
                <path className="wordrect" d={path} />
              </Tooltip>
            );
          })}
        </svg>
      </div>
    </div>
  );
}

export default function StartPage() {
  const [, { raiseAlert, openTab }] = use$ui();
  const [
    { files, socketRef, parse_pages_result, waitingTicket },
    { updateFiles, set_parse_pages_result, SET_WAITING_TICKET },
  ] = use$data();
  const [zoomed, setZoomed] = useState(null);
  const [activeFile, setActiveFile] = useState(null);

  const {
    send,
    success: sendSuccessful,
    response,
  } = useSendFiles({ updateFiles });

  const {
    doFetch: _requestRects,
    response: requestRectsResult,
    error : requestRectsError,
    isLoading,
    isRequestingRects,
  } = useFetch({
    method: "post",
    url: links.extract,
  });

  const requestRects = useCallback(throttle(_requestRects, 100), [])

  useEffect(() => {
    console.log(requestRectsResult)
    if (waitingTicket && !isRequestingRects && requestRectsResult && requestRectsResult.data && requestRectsResult.data.ticket == waitingTicket) {
      raiseAlert({ text: "Данные успешно обработаны", type: "success" });
      SET_WAITING_TICKET(null);
      set_parse_pages_result(
        files
          .filter((file) => file.visible)
          .map(({ id }, index) => ({
            id: id,
            rects: requestRectsResult.data.rects[index],
          }))
      );
    }
  }, [isRequestingRects, requestRectsResult, waitingTicket]);

useEffect(() => {
    if (!isRequestingRects && requestRectsError) {
        setTimeout(() => {
            requestRects({ data: {ticket: waitingTicket} });
        },1000)
    }
  }, [isRequestingRects, requestRectsError, waitingTicket]);

  useEffect(() => {
    if (waitingTicket) {
      socketRef.current.emit("wait", { task: waitingTicket });
      requestRects({ data: {ticket: waitingTicket} });
    }
    // const events = {
    //   "parse_pages-error": () => {
    //     raiseAlert({
    //       text: "при обработке данных возникла ошибка",
    //       type: "error",
    //     });
    //   },
    //   parse_pages: (message) => {
    //     console.log('message', message)
    //     if (message != waitingTicket) {
    //       console.log(
    //         "пришли данные по тикету",
    //         message.task,
    //         "сейчас ожидается",
    //         waitingTicket
    //       );
    //       return;
    //     }
    //     requestRects({ data: {ticket: message} });

    //   },
    // };
    // for (const eventName in events)
    //   socketRef.current.on(eventName, events[eventName]);

    // return () => {
    //   for (const eventName in events)
    //     socketRef.current.off(eventName, events[eventName]);
    // };
  }, [waitingTicket, requestRects]);

  useEffect(() => {
    if (response) SET_WAITING_TICKET(response.data);
  }, [response]);

  const onDrop = useCallback((acceptedFiles) => {
    updateFiles({ type: "add", files: acceptedFiles, updateFiles, raiseAlert });
  }, []);

  useEffect(() => {
    if (!parse_pages_result || parse_pages_result.length == 0) send(files);
  }, []);

  return (
    <div
      width="100vw"
      height="100vh"
      className="pageRoot"
      style={{ maxHeight: "100vh" }}
    >
      <Fab
        onClick={(event) => {
          prevent(event);
          openTab("startPage");
        }}
        variant="extended"
        className="fab-prev"
      >
        <ArrowBackIcon />
        Вернуться к загрузке файлов
      </Fab>
      <Fab
        onClick={(event) => {
          prevent(event);
          send(files);
        }}
        disabled={files.length == 0}
        variant="extended"
        className="fab-next"
      >
        <CheckBoxIcon />
        Перевыполнить анализ
      </Fab>
      <Dialog
        open={!!zoomed}
        onClose={(event) => {
          event.stopPropagation();
          setZoomed(null);
        }}
      >
        <img src={zoomed} onClick={prevent} />
      </Dialog>

      <div className="dropzone-calm_root">
        <div className="dropzone-header"></div>
        <div
          onClick={prevent}
          className="dropzone-content-container"
          style={{ display: "flex" }}
        >
          <div
            style={{
              flexGrow: 1,
              flexBasis: "30vw",
              position: "relative",
              overflowY: "auto",
            }}
          >
            <DNDList
              items={files}
              setItems={(items) => updateFiles({ type: "set", files: items })}
              getListStyle={getListStyle}
              getItemStyle={getItemStyle}
              itemClickHandler={(file) => {
                setActiveFile(file.id);
              }}
              content={ImageCardConstructorFactory({
                setZoomed,
                updateFiles,
                canRemove: false,
              })}
            />
          </div>
          <div style={{ flexGrow: 2, flexBasis: "70vw", position: "relative" }}>
            {activeFile && <FileCard activeFile={activeFile} />}
          </div>
        </div>
      </div>
    </div>
  );
}
