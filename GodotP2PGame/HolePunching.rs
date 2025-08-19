use std::cmp::min;
use std::io;

use std::io::ErrorKind;
use std::mem;
use std::net::SocketAddr;
use std::net::UdpSocket;

#[repr(u8)]
#[derive(Debug)]
pub enum PacketType {
    RegisterEndpoint = 200,
    GetEndpoint = 201,
    HolePunch = 202,
    TestChat = 203,
}

impl From<u8> for PacketType {
    fn from(v: u8) -> Self {
        match v {
            200 => PacketType::RegisterEndpoint,
            201 => PacketType::GetEndpoint,
            202 => PacketType::HolePunch,
            203 => PacketType::TestChat,
            _ => panic!("Unknown packet type"),
        }
    }
}

#[derive(Debug)]
struct Chat {
    len: u32,
    message: [char; 100],
}

impl Chat {
    fn to_string(&self) -> String {
        let mut s = String::new();
        for i in 0..self.len as usize {
            s.push(self.message[i]);
        }
        s
    }
}

#[derive(Debug, Clone)]
struct Endpoint {
    addr: SocketAddr,
}

//Error Type for unpacking
#[derive(Debug)]
pub enum UnpackError {
    InvalidSize,
}

pub fn unpack<T>(data: &[u8]) -> Result<(T, u32), UnpackError> {
    if data.len() < mem::size_of::<T>() {
        print!("Invalid size: {} <-> {}", data.len(), mem::size_of::<T>());

        return Err(UnpackError::InvalidSize);
    }

    let my_struct: T;
    let raw: &[u8] = &data[..mem::size_of::<T>()];
    unsafe {
        let ptr = raw.as_ptr() as *const T;
        my_struct = ptr.read();
    }
    Ok((my_struct, mem::size_of::<T>() as u32))
}

pub fn pack<T>(data: &T, packet_type: PacketType) -> Vec<u8> {
    let mut packet = Vec::new();
    packet.push(packet_type as u8);
    let data_ptr: *const u8 = data as *const T as *const u8;
    let data_size = mem::size_of::<T>();
    unsafe {
        let data_slice = std::slice::from_raw_parts(data_ptr, data_size);
        packet.extend_from_slice(data_slice);
    }
    packet
}

pub fn start_udp(port: u16) -> io::Result<UdpSocket> {
    let socket = UdpSocket::bind(format!("0.0.0.0:{}", port))?;
    socket.set_nonblocking(true)?;
    Ok(socket)
}

pub fn send_bytes(socket: Option<&UdpSocket>, packet: &[u8], addr: &str) {
    let r = socket.unwrap().send_to(&packet, addr);
    if r.is_err() {
        println!("Error sending packet: {:?}", r.err());
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("Usage: {} [server|client]", args[0]);
        return;
    }

    match args[1].as_str() {
        "client" => test_client(),
        _ => println!("Start Server"),
    }

    let socket = start_udp(55555).unwrap();

    let mut clients: Vec<Endpoint> = Vec::new();

    loop {
        let mut buf = [0; 1024];
        match socket.recv_from(&mut buf) {
            Ok(((amt, src))) => {
                let packet_type = PacketType::from(buf[1]);
                let data = &buf[1..amt];
                match packet_type {
                    PacketType::RegisterEndpoint => {
                        println!("RegisterEndpoint: {}", src.to_string());

                        let is_match = clients.iter().filter(|x| x.addr != src).take(1).next();
                        if let Some(ohter) = is_match {
                            let mut packet = pack::<Endpoint>(&ohter, PacketType::GetEndpoint);

                            println!("Sending GetEndpoint to: {:?}", ohter);

                            packet.insert(0, (packet.len() + 1) as u8);
                            send_bytes(Some(&socket), &packet, src.to_string().as_str());

                            let mut packet = pack::<Endpoint>(&Endpoint { addr: src }, PacketType::GetEndpoint);
                            packet.insert(0, (packet.len() + 1) as u8);

                            send_bytes(Some(&socket), &packet, ohter.addr.to_string().as_str());

                            clients.clear();
                        } else {
                            println!("No other clients");
                            clients.push(Endpoint { addr: src });
                        }
                    }
                    _ => {
                        println!("Unknown packet type: {:?}", packet_type);
                    }
                }
            }
            Err(e) => match e.kind() {
                ErrorKind::WouldBlock => {}
                _ => {
                    println!("Error: {:?}", e);
                }
            },
        }
    }
}

fn test_client() {
    let socket = start_udp(0).unwrap();
    let mut packet = pack(&0u8, PacketType::RegisterEndpoint);
    packet.insert(0, (packet.len() + 1) as u8);
    send_bytes(Some(&socket), &packet, "13.124.53.124:55555");

    let mut peer_endpoint: Option<Endpoint> = None;

    loop {
        let mut buf: [u8; 1024] = [0; 1024];
        match socket.recv_from(&mut buf) {
            Ok((amt, src)) => {
                let packet_type = PacketType::from(buf[1]);
                let data = &buf[2..amt];
                match packet_type {
                    PacketType::GetEndpoint => {
                        println!("buf: {:?}", buf);
                        let (ep, _) = unpack::<Endpoint>(data).unwrap();
                        if peer_endpoint.is_none() {
                            peer_endpoint = Some(ep.clone());

                            //Hole Punch
                            let mut packet = pack::<u8>(&0, PacketType::HolePunch);
                            packet.insert(0, (packet.len() + 1) as u8);
                            send_bytes(Some(&socket), &packet, peer_endpoint.as_ref().unwrap().addr.to_string().as_str());

                            println!("GetEndpoint: {:?}", ep);
                        }
                    }
                    PacketType::TestChat => {
                        let (chat, _) = unpack::<Chat>(data).unwrap();
                        println!("Chat: {}", chat.to_string());
                    }
                    _ => {
                        println!("Unknown packet type: {:?}", packet_type);
                    }
                }
            }
            Err(e) => match e.kind() {
                ErrorKind::WouldBlock => {
                }
                _ => {
                    println!("Error: {:?}", e);
                }
            },
        }
        if let Some(peer) = &peer_endpoint {
            let mut input = String::new();
            
            io::stdin().read_line(&mut input).unwrap();
            let len = input.as_bytes().len() as u32;
            let mut text: Chat = Chat { len: len, message: ['\0'; 100] };
            for i in 0..min(len, 100) as usize {
                text.message[i] = input.as_bytes()[i] as char;
            }
            let mut packet = pack::<Chat>(&text, PacketType::TestChat);
            packet.insert(0, (packet.len() + 1) as u8);
            send_bytes(Some(&socket), &packet, peer.addr.to_string().as_str());
        }
    }
}
