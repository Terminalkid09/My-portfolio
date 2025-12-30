import discord
from discord.ext import commands
import phonenumbers
from phonenumbers import geocoder, carrier, number_type, PhoneNumberType

class PhoneLookup(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def phone(self, ctx, number: str):
        """Analizza un numero di telefono"""

        try:
            parsed = phonenumbers.parse(number, None)
        except Exception:
            await ctx.send("❌ Numero non valido. Usa il formato internazionale, esempio: `+393401234567`")
            return

        if not phonenumbers.is_valid_number(parsed):
            await ctx.send("❌ Numero non valido o inesistente.")
            return

        country = geocoder.description_for_number(parsed, "it")
        operator = carrier.name_for_number(parsed, "it")

        type_map = {
            PhoneNumberType.MOBILE: "📱 Mobile",
            PhoneNumberType.FIXED_LINE: "☎️ Fisso",
            PhoneNumberType.FIXED_LINE_OR_MOBILE: "📞 Fisso/Mobile",
            PhoneNumberType.VOIP: "🌐 VoIP",
            PhoneNumberType.TOLL_FREE: "🆓 Numero Verde",
            PhoneNumberType.PREMIUM_RATE: "💰 Premium",
        }

        num_type = type_map.get(number_type(parsed), "❓ Sconosciuto")

        embed = discord.Embed(
            title="📞 Phone Number OSINT",
            color=discord.Color.blue()
        )

        embed.add_field(name="🔹 Numero", value=f"`{number}`", inline=False)
        embed.add_field(name="🌍 Paese", value=country or "N/A", inline=False)
        embed.add_field(name="🏢 Operatore", value=operator or "N/A", inline=False)
        embed.add_field(name="📌 Tipo", value=num_type, inline=False)
        embed.add_field(name="✔️ Valido", value="Sì", inline=False)

        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(PhoneLookup(bot))