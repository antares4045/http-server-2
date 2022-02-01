import React, {useState} from 'react'

import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import ZoomInIcon from '@mui/icons-material/ZoomIn';
import CloseIcon from '@mui/icons-material/Close';
import CheckBoxIcon from '@mui/icons-material/CheckBox';
import VisibilityIcon from '@mui/icons-material/Visibility';
import VisibilityOffIcon from '@mui/icons-material/VisibilityOff';

import Dialog from '@mui/material/Dialog';
import DialogContent from '@mui/material/DialogContent';
import Tooltip from '@mui/material/Tooltip';
import { IconButton } from '@mui/material';


function ZoomableImage({file, name, setZoomed}){
    const [hovered, setHovered] = useState(false)

    function handleClick(event){
        event.stopPropagation()
        setZoomed(file)
    }
    return <img src={file} alt={name} className='zoomableImageContent' onClick={handleClick}/> 
}

const getItemStyle = (isDragging, item) => ({
    border: '1px solid lightgray',
    borderRadius: '1.5rem',
    background: 'white',
    padding : 3,
    margin : 2,
    overflow: 'hidden',
    display: 'flex',
    alignItems: 'center',
    opacity : item.visible ? 1 : 0.5, 
    userSelect: "none",
    background: !isDragging ? "white" : "lightgrey",
});

const getListStyle = isDraggingOver => ({
    display: "flex",
    flexDirection: 'column',
    width: '100%',
    background: isDraggingOver ? "rgba(0,255,255,0.1)" : "transparent",
});


const ImageCardConstructorFactory = ({setZoomed, updateFiles, canRemove=false}) => 
                                ({file, progress, loaded, preview, id, visible}, index) => {
                                    return <React.Fragment>
                                        <div style={{marginRight: 'auto'}}>{canRemove ?<Tooltip title={"Убрать картинку"}>
                                                <IconButton onClick={() => updateFiles({type: 'remove', id})}>
                                                    <CloseIcon/>
                                                </IconButton>
                                            </Tooltip> : null}
                                            <Tooltip title={visible ? "Не использовать картинку" : "Использовать картинку"}>
                                                <IconButton onClick={() => updateFiles({type: 'visibility', id, visible: !visible})}>
                                                    {visible ? <VisibilityIcon/> : <VisibilityOffIcon />}
                                                </IconButton>
                                        </Tooltip>{index+1}) {file.name}</div> 
                                        {loaded 
                                            ? <ZoomableImage file={preview} name={file.name} setZoomed={setZoomed}/>
                                            : `${Math.round(progress * 10000) / 100}%`
                                        }
                                    </React.Fragment>
                                }
export default ImageCardConstructorFactory;
export {getItemStyle, getListStyle}