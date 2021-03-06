#!/usr/bin/env python3

import socket
import threading
from typing import ClassVar, List, Optional, Tuple, Type

PORT=5678

camera_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# By default Linux holds the socket open for several seconds after we spin down.
# That sucks for development purposes.
# There are production reasons not to do this, but I don't care here.
# See https://stackoverflow.com/questions/3229860/what-is-the-meaning-of-so-reuseaddr-setsockopt-option-linux
# or https://stackoverflow.com/questions/337115/setting-time-wait-tcp
camera_server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
camera_server_socket.bind(('', PORT))
print("listening on %s" % PORT)
camera_server_socket.listen()

def to_hex_string(data: bytes) -> str:
	return ":".join("{:02x}".format(c) for c in data)

class VISCAPacket:
	signature: bytes
	signature_mask: bytes

	def __init__(self, sender: int, receiver: int, data: bytes):
		self.sender = sender
		self.receiver = receiver
		self.data = data
	
	def __repr__(self) -> str:
		return "sender {}, receiver {}, data {}".format(self.sender, self.receiver, to_hex_string(data=self.data))

class PacketDecoder:
	def __init__(self) -> None:
		self.packet_definitions: List[Type[VISCAPacket]] = [
			PanTiltPositionInq,
			ZoomPositionInq
		]

	def _bytes_and(self, abytes: bytes, bbytes: bytes) -> bytes:
		return bytes(map(lambda a,b: a & b, abytes, bbytes))

	def decode(self, packet: VISCAPacket) -> Optional[VISCAPacket]:
		for packet_type in self.packet_definitions:
			# this_packet = packet_type(0, 0, 
			masked_data = self._bytes_and(packet.data, packet_type.signature_mask)
			if masked_data == packet_type.signature:
				return packet_type(sender=packet.sender, receiver=packet.receiver, data=packet.data)


class PanTiltPositionInq(VISCAPacket):
	signature = b"\x09\x06\x12"
	signature_mask = b"\xff\xff\xff"

	def __repr__(self) -> str:
		return "PanTiltPositionInq " + super().__repr__()

class ZoomPositionInq(VISCAPacket):
	signature = b"\x09\x04\x47"
	signature_mask = b"\xff\xff\xff"

	def __repr__(self) -> str:
		return "ZoomPositionInq " + super().__repr__()

class ZoomPositionInqResponse(VISCAPacket):
	signature = b"\x50\x00\x00\x00\x00\x00\x00\x00\x00"
	signature_mask = b"\xff\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0"

	def __repr__(self) -> str:
		return "ZoomPositionInqResponse " + super().__repr__()

def parse_visca(accumulated_data: bytes) -> Tuple[bytes, List[VISCAPacket]]:
	result_packets: List[VISCAPacket] = []
	while len(accumulated_data) > 0:
		terminator_index = accumulated_data.find(b"\xff")
		if terminator_index == -1:
			break

		sender = (accumulated_data[0] & 0x70) >> 8
		receiver = (accumulated_data[0] & 0x7)
		data = accumulated_data[1:terminator_index]
		result_packets.append(VISCAPacket(sender=sender, receiver=receiver, data=data))
		accumulated_data = accumulated_data[terminator_index+1:]

	return (accumulated_data, result_packets)

def encode_visca(packet: VISCAPacket) -> bytes:
	output: List[int] = []
	address = 0x80 + (packet.sender << 4) + packet.receiver
	output.append(address)
	output += packet.data
	output.append(0xff)
	return bytes(output)

def listen_thread(camera_client_socket: socket.socket):
	print("listen thread spun up")
	incoming: bytes = b""
	decoder = PacketDecoder()
	while True:
		chunk = camera_client_socket.recv(1024)
		if chunk == b"":
			print("socket died")
			return
		print("rx " + ":".join("{:02x}".format(c) for c in chunk))
		incoming = incoming + chunk

		(incoming, packets) = parse_visca(incoming)

		for packet in packets:
			print("Rx: {}".format(packet))
			decoded = decoder.decode(packet)

			print("  decoded: {}".format(repr(decoded)))

def send_packet(packet: VISCAPacket, socket: socket.socket):
	socket.sendall(encode_visca(packet))

def respond_to(packet: VISCAPacket, socket: socket.socket):
	if isinstance(packet, PanTiltPositionInq):
		send_packet()

while True:
	(camera_client_socket, address) = camera_server_socket.accept()
	print("inbound connection from " + str(address))
	t = threading.Thread(target=listen_thread, args=(camera_client_socket,))
	t.start()
	
