import axios from "axios"
import { useCallback, useEffect, useState } from "react"


const defaultProps = {
    method : 'get',
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
      //"Cache-Control": "no-cache"
    },
    cors: false,
}

export default function useFetch(initialProps={}){
    const [err, setError] = useState(null)
    const [isLoading, setIsLoading] = useState(false)
    const [response, setResponse] = useState(null)
    const [props, setProps] = useState(null)

    useEffect(() => {
        const doRequest = async () => {

            const finalProps = {
                    ...defaultProps,
                    ...initialProps,
                    ...props
                }
            // console.log('axios',  finalProps)
            const resp = await axios(finalProps)
            setResponse(resp)
        }

        if(isLoading)
        {
            (async () => {
                try{
                    await doRequest()
                }catch(err){
                    console.error(err)
                    setError(err)
                }
        setIsLoading(false)
            })()
        }
    }, [isLoading, props])


    const doFetch = useCallback((props={}) => {
        setIsLoading(true)
        setResponse(null)
        setError(null)
        setProps(props)
    }, [setIsLoading, setResponse, setError])

    return {doFetch, error : err, isLoading, response}
  
}