import links from 'constants/links'
import { useCallback, useEffect, useState } from 'react'
import useFetch from './useFetch'

import use$ui from 'store/ui'

export default function useSendFiles({linkName='file_receiver'}){
    const [,{raiseAlert}] = use$ui();
    const [success, setSuccess] = useState(null)
    
    const {doFetch: _send, error, isLoading, response} = useFetch({
        method: 'post',
        headers: {
        'Content-Type': 'multipart/form-data'
        },
        url : links[linkName]
    })

    useEffect(() => {
        if((!!error || !!response) && !isLoading){
            setSuccess(!error)
            if(!!error)
                raiseAlert({text : 'При передаче данных на обработку возникли проблемы', type: 'error'})
            else
                raiseAlert({text : 'Данные переданы на обработку', type: 'info', duration: -1})
        }

    }, [error, isLoading, response])


    const send = useCallback(function send(files){
        const formData = new FormData();
        files.forEach(({file, visible}, index) => {
            if(visible)
                formData.append(`page-${index}`, file)
        })
        _send({data: formData})
    }, [])


    return {send, success, response}
}